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
using v8::Local;
using v8::Number;
using v8::Uint32;
using v8::Int32;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Symbol;
using v8::Value;
using v8::Exception;
using v8::PropertyAttribute;

using namespace std::string_literals;

using std::string;
using std::to_string;
using std::pair;
using std::forward;

// InitClass implementation

template <typename T>
pair<string, T> Constant(const string& name, T&& val) {
  return make_pair(name, forward<T>(val));
}

namespace {

void SetUpClassProp(Local<Object> exports, Local<FunctionTemplate> tpl) {}

template <typename T, class... Rest>
typename std::enable_if<std::is_enum<T>::value, void>::type
SetUpClassProp(
          Local<Object> exports, Local<FunctionTemplate> tpl,
          const pair<string, T>& constant, Rest&&... rest);
template <typename T, class... Rest>
typename std::enable_if<!std::is_enum<T>::value, void>::type
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
typename std::enable_if<std::is_enum<T>::value, void>::type
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
typename std::enable_if<!std::is_enum<T>::value, void>::type
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
  if (0 == max_num_args) return true;

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

}  // namespace node_addon
}  // namespace at
