/// @copyright © 2017 Airtime Media.  All rights reserved.

#include <stdexcept>
#include <vector>
#include <functional>
#include <stdexcept>

#include "addon_util.h"

namespace at {
namespace node_addon {

using namespace std;

using v8::Isolate;
using v8::HandleScope;
using v8::Local;
using v8::Value;
using v8::Boolean;
using v8::Int32;
using v8::Uint32;
using v8::Number;
using v8::Integer;
using v8::String;
using v8::Object;
using v8::Function;


string ToString(const Local<Value> v8str) {
  String::Utf8Value utf8(v8str);
  if (0 < utf8.length()) return string(*utf8, utf8.length());
  return "";
}

string Inspect(Local<Value> value) {
  auto detail = "unknown"s;
  auto maybeString = value->ToDetailString(Isolate::GetCurrent()->GetCurrentContext());
  Local<String> d;
  if (maybeString.ToLocal(&d)) {
    detail = ToString(d);
  }
  return detail;
}

void ThrowException(Local<Value>(* ex)(Local<String>), const string& msg) {
  auto isolate = Isolate::GetCurrent();
  isolate->ThrowException(ex(String::NewFromUtf8(isolate, msg.c_str())));
}

void ThrowException(
            const FunctionCallbackInfo<Value>& args, Local<Value>(* ex)(Local<String>), const string& msg) {
  auto m = Inspect(args.Holder()) + " " + msg;
  ThrowException(ex, m);
}

int32_t ToInt32(Local<Value> val) {
  auto isolate = Isolate::GetCurrent();
  auto maybeValue = val->ToInt32(isolate->GetCurrentContext());
  Local<Int32> value;
  if (maybeValue.ToLocal(&value)) {
    return value->Value();
  }
  throw logic_error("Value is not int32 but " + Inspect(val));
}

uint32_t ToUint32(Local<Value> val) {
  auto isolate = Isolate::GetCurrent();
  auto maybeValue = val->ToUint32(isolate->GetCurrentContext());
  Local<Uint32> value;
  if (maybeValue.ToLocal(&value)) {
    return value->Value();
  }
  throw logic_error("Value is not uint32 but " + Inspect(val));
}

double ToDouble(Local<Value> val) {
  auto isolate = Isolate::GetCurrent();
  auto maybeValue = val->ToNumber(isolate->GetCurrentContext());
  Local<Number> value;
  if (maybeValue.ToLocal(&value)) {
    return value->Value();
  }
  throw logic_error("Value is not double but " + Inspect(val));
}

bool ToBool(Local<Value> val) {
  return val->IsTrue();
}

pair<string, FunctionCallback> Prototype(const string& name, FunctionCallback func) {
  return make_pair(name, func);
}

Local<Boolean> ToLocalBoolean(bool value) {
  return Boolean::New(Isolate::GetCurrent(), value);
}

Local<Value> ToLocalValue(bool value) {
  return ToLocalBoolean(value);
}

Local<Integer> ToLocalInteger(int32_t value) {
  return Integer::New(Isolate::GetCurrent(), value);
}

Local<Integer> ToLocalInteger(int64_t value) {
  return ToLocalInteger(static_cast<int32_t>(value));
}

Local<Value> ToLocalValue(int32_t value) {
  return ToLocalInteger(value);
}

Local<Value> ToLocalValue(int64_t value) {
  return ToLocalInteger(value);
}

Local<Integer> ToLocalInteger(uint32_t value) {
  return Integer::NewFromUnsigned(Isolate::GetCurrent(), value);
}

Local<Integer> ToLocalInteger(uint64_t value) {
  return ToLocalInteger(static_cast<uint32_t>(value));
}

Local<Value> ToLocalValue(uint32_t value) {
  return ToLocalInteger(value);
}

Local<Value> ToLocalValue(uint64_t value) {
  return ToLocalInteger(static_cast<uint32_t>(value));
}

Local<Number> ToLocalNumber(double value) {
  return Number::New(Isolate::GetCurrent(), value);
}

Local<Value> ToLocalValue(double value) {
  return ToLocalNumber(value);
}

Local<String> ToLocalString(const string& value) {
  return String::NewFromUtf8(Isolate::GetCurrent(), value.c_str());
}

Local<Value> ToLocalValue(const string& value) {
  return ToLocalString(value);
}

Local<Value> ToLocalValue(const V8Exception& ex) {
  return ex.ToV8();
}

Local<Object> NewV8Instance(Persistent<Function>& constructor, const FunctionCallbackInfo<Value>& args) {
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

// BlockingWorker

BlockingWorker::BlockingWorker()
  : promise_() {
  work_.data = this;
}

BlockingWorker* BlockingWorker::New() {
  return new BlockingWorker();
}

void BlockingWorker::StartWork(function<void()> after_fn) {
  if (queued_) return;
  after_fn_ = after_fn;
  auto r = uv_queue_work(uv_default_loop(), &work_, KeepAliveUntilFuture, Finish);
  if (0 != r) {
    // what to do?
  }
  queued_ = true;
}

void BlockingWorker::StopWork() {
  // unblock the work
  promise_.set_value(true);
}

void BlockingWorker::Finish(uv_work_t* work, int status) {
  auto self = reinterpret_cast<BlockingWorker*>(work->data);
  assert(self);

  if (self->after_fn_) {
    self->after_fn_();
  }
  delete self; 
}

BlockingWorker::~BlockingWorker() {
}

void BlockingWorker::KeepAliveUntilFuture(uv_work_t* work) {
  auto self = reinterpret_cast<BlockingWorker*>(work->data);
  assert(self);
  // blocks the work
  self->promise_.get_future().get();
}

// -------------------------------

// CallbackInvokerBase

CallbackInvokerBase::CallbackInvokerBase()
  : worker_(BlockingWorker::New())
  , to_call_(false) {
}

CallbackInvokerBase::CallbackInvokerBase(PersistentFunctionCopyable cb_func) {
  SetCallbackFn(cb_func);
}

CallbackInvokerBase::CallbackInvokerBase(Local<Function> cb_func) {
  SetCallbackFn(cb_func);
}

void CallbackInvokerBase::SetCallbackFn(PersistentFunctionCopyable cb_func) {
  if (to_call_) return;  // duplicate and too late. ignore it

  cb_func_.Reset(Isolate::GetCurrent(), cb_func);
  worker_->StartWork([this](){ Finish(); });
}

void CallbackInvokerBase::SetCallbackFn(v8::Local<v8::Function> cb_func) {
  SetCallbackFn(PersistentFunctionCopyable(Isolate::GetCurrent(), cb_func));
}

void CallbackInvokerBase::Stop() {
  if (to_call_) {
    // race. will stop after the call
    return;
  }
  FinishWorker();
}

void CallbackInvokerBase::FinishWorker() {
  if (worker_) {
    worker_->StopWork();  // it suicides
    worker_ = nullptr;
  }
}

CallbackInvokerBase::~CallbackInvokerBase() {
  FinishWorker();
}

void CallbackInvokerBase::Finish() {
  FinishImpl();
  CleanUp();
}

void CallbackInvokerBase::Invoke(PersistentFunctionCopyable fn, const vector<Local<Value>>& args) {
  vector<Local<Value>> copy_args = args;
  Invoke(fn, move(copy_args));
}

void CallbackInvokerBase::Invoke(PersistentFunctionCopyable fn, vector<Local<Value>>&& args) {
  auto isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  fn.Get(isolate)->Call(isolate->GetCurrentContext()->Global(), static_cast<int>(args.size()), args.data());
}

void CallbackInvokerBase::CleanUp() {
  cb_func_.Reset();
}

// -------------------------------

// EventEmitterBase

EventEmitterBase::EventEmitterBase()
  : worker_(BlockingWorker::New()) {
}

void EventEmitterBase::Start() {
  lock_guard<mutex>  guard(mutex_);
  if (!worker_) worker_ = BlockingWorker::New();
  worker_->StartWork([this](){ Finish(); });
}

void EventEmitterBase::AddListener(PersistentFunctionCopyable cb_func) {
  lock_guard<mutex>  guard(mutex_);
  listeners_.emplace_back(cb_func);
}

void EventEmitterBase::AddListener(v8::Local<v8::Function> cb_func) {
  AddListener(PersistentFunctionCopyable(Isolate::GetCurrent(), cb_func));
}

void EventEmitterBase::Stop() {
  lock_guard<mutex>  guard(mutex_);
  if (worker_) worker_->StopWork();  // it suicides
  worker_ = nullptr;
}

EventEmitterBase::~EventEmitterBase() {
  if (worker_) {
    worker_->StopWork();
  }
  CleanUp();
}

vector<PersistentFunctionCopyable> EventEmitterBase::CopyListeners() {
  lock_guard<mutex>  guard(mutex_);
  return listeners_;
}

void EventEmitterBase::Finish() {
  FinishImpl();
  CleanUp();
}

void EventEmitterBase::CleanUp() {
  lock_guard<mutex>  guard(mutex_);
  for (auto& lis : listeners_) {
    lis.Reset();
  }
  listeners_.clear();
}

}  // namespace node_addon
}  // namespace at
