/// @copyright © 2017 Airtime Media.  All rights reserved.

#include <stdexcept>
#include <vector>
#include <functional>
#include <stdexcept>

#include "addon_util.h"

namespace at {
namespace node_addon {

using std::to_string;
using std::logic_error;
using std::vector;
using std::function;

using v8::Isolate;
using v8::Local;
using v8::Value;
using v8::Boolean;
using v8::Int32;
using v8::Uint32;
using v8::Number;
using v8::Integer;
using v8::String;
using v8::Function;


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

bool Util::IsV8EnumType(const v8::Local<v8::Value>& val) {
  return val->IsInt32();
}

Util::EnumType Util::ToEnumType(Isolate* isolate, const Local<Value>& val) {
  return ToInt32(isolate, val);
}

pair<string, FunctionCallback> Util::Prototype(const string& name, FunctionCallback func) {
  return make_pair(name, func);
}

pair<string, Util::EnumType> Util::Enum(const string& name, EnumType val) {
  return make_pair(name, val);
}

Local<Boolean> Util::ToLocalBoolean(Isolate* isolate, bool value) {
  return Boolean::New(isolate, value);
}

Local<Value> Util::ToLocalValue(Isolate* isolate, bool value) {
  return ToLocalBoolean(isolate, value);
}

Local<Integer> Util::ToLocalInteger(Isolate* isolate, int32_t value) {
  return Integer::New(isolate, value);
}

Local<Integer> Util::ToLocalInteger(Isolate* isolate, int64_t value) {
  return ToLocalInteger(isolate, static_cast<int32_t>(value));
}

Local<Value> Util::ToLocalValue(Isolate* isolate, int32_t value) {
  return ToLocalInteger(isolate, value);
}

Local<Value> Util::ToLocalValue(Isolate* isolate, int64_t value) {
  return ToLocalInteger(isolate, value);
}

Local<Integer> Util::ToLocalInteger(Isolate* isolate, uint32_t value) {
  return Integer::NewFromUnsigned(isolate, value);
}

Local<Integer> Util::ToLocalInteger(Isolate* isolate, uint64_t value) {
  return ToLocalInteger(isolate, static_cast<uint32_t>(value));
}

Local<Value> Util::ToLocalValue(Isolate* isolate, uint32_t value) {
  return ToLocalInteger(isolate, value);
}

Local<Value> Util::ToLocalValue(Isolate* isolate, uint64_t value) {
  return Uint32::NewFromUnsigned(isolate, static_cast<uint32_t>(value));
}

Local<Number> Util::ToLocalNumber(Isolate* isolate, double value) {
  return Number::New(isolate, value);
}

Local<Value> Util::ToLocalValue(Isolate* isolate, double value) {
  return ToLocalNumber(isolate, value);
}

Local<String> Util::ToLocalString(Isolate* isolate, const std::string& value) {
  return String::NewFromUtf8(isolate, value.c_str());
}

Local<Value> Util::ToLocalValue(Isolate* isolate, const std::string& value) {
  return ToLocalString(isolate, value);
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

// -------------------------------

Notifier::Notifier() {
  auto ret = uv_async_init(uv_default_loop(), &async_, DoNotify);
  if (0 != ret) {
    throw std::runtime_error("Notifier uv_async_init failed with " + to_string(ret));
  }
  async_.data = this;
}

void Notifier::Notify(Local<Function> listener, const vector<Local<Value>>& args) {
    Notify(PersistentFunctionCopyable(Isolate::GetCurrent(), listener), args);
}

void Notifier::Notify(Persistent<Function> listener, const vector<Local<Value>>& args) {
  Notify(listener.Get(Isolate::GetCurrent()), args);
}

void Notifier::Notify(PersistentFunctionCopyable listener, const vector<Local<Value>>& args) {
  auto isolate = Isolate::GetCurrent();
  { LockGuard guard(funcs_mutex_);
    funcs_.emplace_back(make_pair(listener, vector<PersistentValueCopyable>()));
    auto& persistent_args = funcs_.back().second;
    persistent_args.reserve(args.size());
    for (const auto& a : args) {
      persistent_args.emplace_back(isolate, a);
    }
  }

std::cout << "###### async_send pending " << async_.pending << " loop " << async_.loop << "\n";

  auto ret = uv_async_send(&async_);
  if (0 != ret) {
    throw std::runtime_error("Notifier uv_async_send failed with " + to_string(ret));
  }
}


Notifier::~Notifier() {
  uv_close(reinterpret_cast<uv_handle_t*>(&async_), nullptr);
}

void Notifier::DoNotify(uv_async_t* handle) {

std::cout << "###### DoNotify\n";

  auto self = static_cast<Notifier*>(handle->data);
  assert(self);

  auto isolate = Isolate::GetCurrent();
  auto context = isolate->GetCurrentContext();
  
  FuncList func_list;
  { LockGuard guard(self->funcs_mutex_);
    func_list.swap(self->funcs_);
  }
  for (const auto& func_args : func_list) {
    const auto& persistent_args = func_args.second;
    int argc = persistent_args.size();
    vector<Local<Value>> args;
    for (auto const& a : persistent_args) {
      args.emplace_back(a.Get(isolate));
    }
    func_args.first.Get(isolate)->Call(context, Null(isolate), argc, args.data()).IsEmpty();
  }
}

}  // namespace node_addon
}  // namespace at
