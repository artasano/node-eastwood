// Copyright Airtime Media, Inc. 2017

#include <stdexcept>
#include <vector>
#include <functional>

#include "addon_util.h"

namespace at {
namespace node_addon {

using std::to_string;
using std::logic_error;
using std::vector;
using std::function;

string Util::ToString(const Local<Value> v8str) {
  String::Utf8Value utf8(v8str);
  if (0 < utf8.length()) return string(*utf8, utf8.length());
  return "";
}

string Util::Inspect(Isolate* isolate, Local<Value> value) {
  auto detail = "unknown"s;
  auto maybeString = value->ToDetailString(isolate->GetCurrentContext());
  Local<String> d;
  if (maybeString.ToLocal(&d)) {
    detail = ToString(d);
  }
  return detail;
}

void Util::ThrowException(Isolate* isolate, Local<Value>(* ex)(Local<String>), const string& msg) {
  assert(isolate);
  isolate->ThrowException(ex(String::NewFromUtf8(isolate, msg.c_str())));
}

void Util::ThrowException(
            const FunctionCallbackInfo<Value>& args, Local<Value>(* ex)(Local<String>), const string& msg) {
  auto isolate = args.GetIsolate();
  auto m = Inspect(isolate, args.Holder()) + " " + msg;
  ThrowException(isolate, ex, m);
}

int32_t Util::ToInt32(Isolate* isolate, Local<Value> val) {
  assert(isolate);
  auto maybeValue = val->ToInt32(isolate->GetCurrentContext());
  Local<Int32> value;
  if (maybeValue.ToLocal(&value)) {
    return value->Value();
  }
  throw logic_error("Value is not int32 but " + Inspect(isolate, val));
}

uint32_t Util::ToUint32(Isolate* isolate, Local<Value> val) {
  assert(isolate);
  auto maybeValue = val->ToUint32(isolate->GetCurrentContext());
  Local<Uint32> value;
  if (maybeValue.ToLocal(&value)) {
    return value->Value();
  }
  throw logic_error("Value is not uint32 but " + Inspect(isolate, val));
}

double Util::ToDouble(Isolate* isolate, Local<Value> val) {
  assert(isolate);
  auto maybeValue = val->ToNumber(isolate->GetCurrentContext());
  Local<Number> value;
  if (maybeValue.ToLocal(&value)) {
    return value->Value();
  }
  throw logic_error("Value is not double but " + Inspect(isolate, val));
}

bool Util::ToBool(Isolate* isolate, Local<Value> val) {
  assert(isolate);
  return val->IsTrue();
}

pair<string, FunctionCallback> Util::Prototype(const string& name, FunctionCallback func) {
  return make_pair(name, func);
}

pair<string, int> Util::Enum(const string& name, int val) {
  return make_pair(name, val);
}

Local<Object> Util::NewV8Instance(Persistent<Function>& constructor, const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  // copy args to its constructor
  vector<Local<Value>> isolated_args;
  auto isolated_argc = args.Length();
  isolated_args.reserve(isolated_argc);
  for (auto i = 0 ; i < isolated_argc ; ++i) {
    isolated_args[i] = args[i];
  }
  auto ctor = Local<Function>::New(isolate, constructor);
  return ctor->NewInstance(isolate->GetCurrentContext(), isolated_argc, isolated_args.data()).ToLocalChecked();
}

void Util::NewCppInstance(
        const FunctionCallbackInfo<Value>& args, const string& class_name, function<Local<Object>()> create_and_wrap_func) {
  auto isolate = args.GetIsolate();

  if (args.IsConstructCall()) {
    // construct
    args.GetReturnValue().Set(create_and_wrap_func());
  } else {
    ThrowException(args, Exception::SyntaxError, "Use 'new' to instantiate");
    return;
  }
}

}  // namespace node_addon
}  // namespace at
