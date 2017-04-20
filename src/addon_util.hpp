/// @copyright © 2017 Airtime Media.  All rights reserved.

#ifndef AT_ADDON_UTIL_H_
# error Must be implicitly included in addon_util.h
#endif

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

namespace {

void SetUpClassProp(Local<Object> exports, Local<FunctionTemplate> tpl) {}

template <class... Rest>
void SetUpClassProp(
            Local<Object> exports, Local<FunctionTemplate> tpl,
            const pair<string, int>& enm, Rest&&... rest);

template <class... Rest>
void SetUpClassProp(
            Local<Object> exports, Local<FunctionTemplate> tpl,
            const pair<string, FunctionCallback>& method, Rest&&... rest) {
  auto isolate = exports->GetIsolate();
  NODE_SET_PROTOTYPE_METHOD(tpl, method.first.c_str(), method.second);
  SetUpClassProp(exports, tpl, forward<Rest>(rest)...);
}

template <class... Rest>
void SetUpClassProp(
          Local<Object> exports, Local<FunctionTemplate> tpl,
          const pair<string, Util::EnumType>& enm, Rest&&... rest) {
  auto isolate = exports->GetIsolate();
  auto context = isolate->GetCurrentContext();
  tpl->GetFunction()->DefineOwnProperty(context,
            String::NewFromUtf8(isolate, enm.first.c_str()),
            Int32::New(isolate, enm.second),
            static_cast<PropertyAttribute>(v8::ReadOnly | v8::DontDelete)).FromJust();

  SetUpClassProp(exports, tpl, forward<Rest>(rest)...);
}

}  // anonymous namespace

template <class... Prop>  // Prototype(..) or Enum(..)
void Util::InitClass(
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
  auto isolate = Isolate::GetCurrent();

  auto err_msg = ""s;
  if (!check(args[N], err_msg)) {
    Util::ThrowException(args, Exception::TypeError,
        context + ": Wrong argument at " + to_string(N) + ". "
        + (err_msg.empty()
           ? "given " + Util::Inspect(args[N])
           : err_msg));
    return false;
  }
  if (args.Length() <= N + 1) return true;
  return CheckArg<N+1>(context, args, forward<RestOfCheckFunc>(rest)...);
}

}  // anonymous namespace

template <class... CheckFunc> // CheckFunc: pair<bool, string>(const Local<Value>)
bool Util::CheckArgs(const string& context, const FunctionCallbackInfo<Value>& args,
  size_t min_num_args, size_t max_num_args,
  CheckFunc&&... checks) {

  auto isolate = Isolate::GetCurrent();

  // Check the number of arguments passed.
  if (args.Length() < min_num_args)  {
    ThrowException(args, Exception::TypeError,
        context + ": Needs " + to_string(min_num_args) + " args but given " + to_string(args.Length()));
    return false;
  }
  if (max_num_args < args.Length()) {
    ThrowException(args, Exception::TypeError,
        context + ": Takes " + to_string(max_num_args) + " args but given " + to_string(args.Length()));
    return false;
  }
  if (0 == max_num_args) return true;

  // Check the argument types
  return CheckArg<0>(context, args, forward<CheckFunc>(checks)...);
}

template <class T>
bool Util::NewCppInstance(const FunctionCallbackInfo<Value>& args, T* obj) {
  auto isolate = args.GetIsolate();

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
