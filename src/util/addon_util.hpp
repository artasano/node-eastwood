/// @copyright © 2017 Airtime Media.  All rights reserved.

#ifndef AT_ADDON_UTIL_H_
# error Must be implicitly included in addon_util.h
#endif

#include <type_traits>
#include <typeinfo>

namespace at {
namespace node_addon {

using v8::FunctionCallbackInfo;
using v8::FunctionCallback;
using v8::Context;
using v8::Function;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::HandleScope;
using v8::Local;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Value;
using v8::Null;
using v8::Exception;
using v8::PropertyAttribute;

using namespace std::string_literals;

using std::string;
using std::to_string;
using std::pair;
using std::forward;
using std::vector;
using std::tuple;
using std::tuple_size;
using std::tuple_element;
using std::enable_if;
using std::shared_ptr;
using std::make_shared;
using std::remove_reference;


// InitClass implementation

template <typename T>
pair<string, T> Constant(const string& name, T&& val) {
  return make_pair(name, forward<T>(val));
}

namespace {

void SetUpClassProp(Local<Object> exports, Local<FunctionTemplate> tpl) {}

template <typename T, class... Rest>
typename enable_if<std::is_enum<T>::value, void>::type
SetUpClassProp(
          Local<Object> exports, Local<FunctionTemplate> tpl,
          const pair<string, T>& constant, Rest&&... rest);
template <typename T, class... Rest>
typename enable_if<!std::is_enum<T>::value, void>::type
SetUpClassProp(
          Local<Object> exports, Local<FunctionTemplate> tpl,
          const pair<string, T>& constant, Rest&&... rest);

template <class... Rest>
void SetUpClassProp(
            Local<Object> exports, Local<FunctionTemplate> tpl,
            const pair<string, FunctionCallback>& method, Rest&&... rest) {
  NODE_SET_PROTOTYPE_METHOD(tpl, method.first.c_str(), method.second);
  SetUpClassProp(exports, tpl, forward<Rest>(rest)...);
}

template <typename T, class... Rest>
typename enable_if<std::is_enum<T>::value, void>::type
SetUpClassProp(
          Local<Object> exports, Local<FunctionTemplate> tpl,
          const pair<string, T>& constant, Rest&&... rest) {
  auto isolate = exports->GetIsolate();
  auto context = isolate->GetCurrentContext();
  tpl->GetFunction()->DefineOwnProperty(context,
            ToLocalString(constant.first.c_str()),
            ToLocalValue(static_cast<int32_t>(constant.second)),
            static_cast<PropertyAttribute>(v8::ReadOnly | v8::DontDelete)).FromJust();

  SetUpClassProp(exports, tpl, forward<Rest>(rest)...);
}


template <typename T, class... Rest>
typename enable_if<!std::is_enum<T>::value, void>::type
SetUpClassProp(
          Local<Object> exports, Local<FunctionTemplate> tpl,
          const pair<string, T>& constant, Rest&&... rest) {
  auto isolate = exports->GetIsolate();
  auto context = isolate->GetCurrentContext();
  tpl->GetFunction()->DefineOwnProperty(context,
            ToLocalString(constant.first.c_str()),
            ToLocalValue(constant.second),
            static_cast<PropertyAttribute>(v8::ReadOnly | v8::DontDelete)).FromJust();

  SetUpClassProp(exports, tpl, forward<Rest>(rest)...);
}

}  // anonymous namespace

template <class... Prop>  // Prototype(..) or Enum(..)
void InitClass(
          Local<Object> exports, const string& class_name,
          void(* new_func)(const FunctionCallbackInfo<Value>&),
          Persistent<Function>& constructor,
          Prop&&... props) {
  auto isolate = exports->GetIsolate();

  // Prepare constructor template
  auto tpl = FunctionTemplate::New(isolate, new_func);
  tpl->SetClassName(String::NewFromUtf8(isolate, class_name.c_str()));
  tpl->InstanceTemplate()->SetInternalFieldCount(sizeof...(Prop));

  // Prototype
  SetUpClassProp(exports, tpl, forward<Prop>(props)...);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, class_name.c_str()), tpl->GetFunction());
}


// CheckArgs implementation

namespace {

template <size_t N>
bool CheckArg(const string& context, const FunctionCallbackInfo<Value>& args) {
  return true;
}

template <size_t N, class CheckFunc, class... RestOfCheckFunc>
bool CheckArg(
        const string& context, const FunctionCallbackInfo<Value>& args,
        CheckFunc&& check, RestOfCheckFunc&&... rest) {

  auto err_msg = ""s;
  if (!check(args[N], err_msg)) {
    ThrowException(args, Exception::TypeError,
        context + ": Wrong argument at " + to_string(N) + ". "
        + (err_msg.empty()
           ? "given " + Inspect(args[N])
           : err_msg));
    return false;
  }
  if (static_cast<size_t>(args.Length()) <= N + 1) return true;
  return CheckArg<N+1>(context, args, forward<RestOfCheckFunc>(rest)...);
}

}  // anonymous namespace

template <class... CheckFunc> // CheckFunc: pair<bool, string>(const Local<Value>)
bool CheckArgs(const string& context, const FunctionCallbackInfo<Value>& args,
                      size_t min_num_args, size_t max_num_args,
                      CheckFunc&&... checks) {

  // Check the number of arguments passed.
  if (static_cast<size_t>(args.Length()) < min_num_args)  {
    ThrowException(args, Exception::TypeError,
        context + ": Needs " + to_string(min_num_args) + " args but given " + to_string(args.Length()));
    return false;
  }
  if (max_num_args < static_cast<size_t>(args.Length())) {
    ThrowException(args, Exception::TypeError,
        context + ": Takes " + to_string(max_num_args) + " args but given " + to_string(args.Length()));
    return false;
  }
  if (0 == args.Length()) return true;

  // Check the argument types
  return CheckArg<0>(context, args, forward<CheckFunc>(checks)...);
}

template <class T>
bool NewCppInstance(const FunctionCallbackInfo<Value>& args, T* obj) {
  if (args.IsConstructCall()) {
    // construct
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
    return true;
  } else {
    ThrowException(args, Exception::SyntaxError, "Use 'new' to instantiate "s + typeid(T).name());
    return false;
  }
}

// CallbackInvoker

template <typename ... Args>
CallbackInvoker<Args...>::CallbackInvoker()
  : CallbackInvokerBase() {
}

template <typename ... Args>
CallbackInvoker<Args...>::CallbackInvoker(PersistentFunctionCopyable cb_func)
  : CallbackInvokerBase(cb_func) {
}

template <typename ... Args>
CallbackInvoker<Args...>::CallbackInvoker(Local<Function> cb_func)
  : CallbackInvokerBase(cb_func) {
}

namespace {

template <size_t Idx, typename ... Args>
void SetSharedPtrTuple(tuple<shared_ptr<Args>...>& tpl) {
}

template <size_t Idx, typename ... Args, typename CallArg, typename ... Rest>
void SetSharedPtrTuple(tuple<shared_ptr<Args>...>& tpl, CallArg&& arg, Rest&&... rest) {
  std::get<Idx>(tpl) = make_shared<CallArg>(forward<CallArg>(arg));
  SetSharedPtrTuple<Idx + 1>(tpl, forward<Rest>(rest)...);
}

template <size_t Idx, typename ... Args, typename ... Rest>
void SetSharedPtrTuple(tuple<shared_ptr<Args>...>& tpl, std::nullptr_t arg, Rest&&... rest) {
  std::get<Idx>(tpl) = nullptr;
  SetSharedPtrTuple<Idx + 1>(tpl, forward<Rest>(rest)...);
}

}  // anonymous namespace

template <typename ... Args>
template <typename ... CallArgs>
void CallbackInvoker<Args...>::Call(CallArgs&&... args) {
  static_assert(sizeof...(CallArgs) <= sizeof...(Args), "Too many args");
  if (to_call().exchange(true)) {
    // already done
    return;
  }
  SetSharedPtrTuple<0>(args_, forward<CallArgs>(args)...);

  // unblock the worker
  FinishWorker();
}

template <typename ... Args>
CallbackInvoker<Args...>::~CallbackInvoker() {
}

namespace {

// helper converting tuple<shared_ptr<T>> to vector<Local<Value>>

template <typename T>
Local<Value> SharedPtrToV8Value(const shared_ptr<T>& sp_val) {
  if (sp_val) {
    return ToLocalValue(*sp_val);
  } else {
    return Null(Isolate::GetCurrent());
  }
}

template <size_t Idx, size_t N, typename... Args>
struct EmplaceV8Arg {
  void operator()(vector<Local<Value>>& v8_args, const tuple<shared_ptr<Args>...>& args) {
    v8_args.emplace_back(SharedPtrToV8Value(std::get<Idx>(args)));
    EmplaceV8Arg<Idx+1, N, Args...>()(v8_args, args);
  }
};

template <size_t N, typename... Args>
struct EmplaceV8Arg<N, N, Args...> {
  void operator()(vector<Local<Value>>& v8_args, const tuple<shared_ptr<Args>...>& args) {
  }
};

template <typename ... Args>
vector<Local<Value>> ToLocalValues(const tuple<shared_ptr<Args>...>& args) {
  vector<Local<Value>> v8_args;
  v8_args.reserve(sizeof...(Args));
  EmplaceV8Arg<0, sizeof...(Args), Args...>()(v8_args, args);
  return v8_args;
}

}  // anonymous namespace

template <typename ... Args>
void CallbackInvoker<Args...>::FinishImpl() {
  if (to_call()) {
    HandleScope scope(Isolate::GetCurrent());
    Invoke(cb_func(), ToLocalValues(args_));
  }
}



// -------------------------------

// EventEmitter

template <typename ... Args>
EventEmitter<Args...>::EventEmitter()
  : EventEmitterBase() {
}

template <typename ... Args>
EventEmitter<Args...>::~EventEmitter() {
}

namespace {

// payload data
template <typename ... Args>
struct EventContext {
  vector<PersistentFunctionCopyable> listeners;
  tuple<shared_ptr<Args>...> args;
  uv_async_t async;
};

}  // anonymous namespace

template <typename ... Args>
template <typename ... EmitArgs>
void EventEmitter<Args...>::Emit(EmitArgs&&... args) {
  auto e = new EventContext<Args...>;
  e->listeners = CopyListeners();

  static_assert(sizeof...(Args) <= sizeof...(EmitArgs), "Too many args");
  SetSharedPtrTuple<0>(e->args, forward<EmitArgs>(args)...);

  uv_async_init(uv_default_loop(), &e->async, EmitEvent);

  e->async.data = e;
  auto r = uv_async_send(&e->async);
  if (0 != r) {
    // what to do?
  }
}

// emit function runs on JS thread
template <typename ... Args>
void EventEmitter<Args...>::EmitEvent(uv_async_t* handle) {
  auto e = static_cast<EventContext<Args...>*>(handle->data);
  assert(e);

  HandleScope scope(Isolate::GetCurrent());
  vector<Local<Value>> v8_args = ToLocalValues(e->args);

  for (auto& lis : e->listeners) {
    CallbackInvokerBase::Invoke(lis, v8_args);
    lis.Reset();
  }
  uv_close(reinterpret_cast<uv_handle_t*>(handle), nullptr);
  delete e;
}


}  // namespace node_addon
}  // namespace at
