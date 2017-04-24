/// @copyright © 2017 Airtime Media.  All rights reserved.

#include <stdexcept>
#include <vector>
#include <functional>
#include <stdexcept>

// TODO(Art): temp
#include <iostream>

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
// TODO(Art): temp
cout << "Starting worker " << this << " " << queued_ << "\n";

  after_fn_ = after_fn;
  uv_queue_work(uv_default_loop(), &work_, KeepAliveUntilFuture, Finish);
  queued_ = true;

// TODO(Art): temp
cout << "Queued " << this << "\n";
}

void BlockingWorker::StopWork() {
// TODO(Art): temp
cout << "Stopping worker " << this << "\n";

  // unblock the work
  promise_.set_value(true);
}

void BlockingWorker::Finish(uv_work_t* work, int status) {
  auto self = reinterpret_cast<BlockingWorker*>(work->data);
  assert(self);

// TODO(Art): temp
cout << "Worker Finish " << self << "\n";

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

// TODO(Art): temp
cout << "Worker Unblocked " << self << "\n";
}

// CallbackInvoker

CallbackInvoker::CallbackInvoker()
  : worker_(BlockingWorker::New()) {
}

CallbackInvoker::CallbackInvoker(PersistentFunctionCopyable cb_func) {
  SetCallbackFn(cb_func);
}

CallbackInvoker::CallbackInvoker(Local<Function> cb_func) {
  SetCallbackFn(cb_func);
}

void CallbackInvoker::SetCallbackFn(PersistentFunctionCopyable cb_func) {
// TODO(Art): temp
cout << "CallbackInvoker SetCallback\n";

  lock_guard<mutex> guard(args_mutex_);
  cb_func_.Reset(Isolate::GetCurrent(), cb_func);
  worker_->StartWork([this](){ Finish(); });
}

void CallbackInvoker::SetCallbackFn(v8::Local<v8::Function> cb_func) {
  SetCallbackFn(PersistentFunctionCopyable(Isolate::GetCurrent(), cb_func));
}

void CallbackInvoker::Call(const vector<v8::Local<v8::Value>>& args) {
// TODO(Art): temp
cout << "CallbackInvoker Call\n";

  lock_guard<mutex>  guard(args_mutex_);
  if (to_call_) {
    // already done
    return;
  }
  to_call_ = true;

  auto isolate = Isolate::GetCurrent();

  args_.reserve(args.size());
  for (const auto& a : args) {
    args_.emplace_back(isolate, a);
  }

  // unblock the worker
  worker_->StopWork();  // it suicides
  worker_ = nullptr;
}

void CallbackInvoker::Stop() {
  lock_guard<mutex>  guard(args_mutex_);
  if (to_call_) {
    // race. will stop after the call
    return;
  }
  if (worker_) {
    worker_->StopWork();  // it suicides
    worker_ = nullptr;
  }
}

CallbackInvoker::~CallbackInvoker() {
// TODO(Art): temp
cout << "~CallbackInvoker\n";
  if (worker_) {
    worker_->StopWork();  // it suicides
    worker_ = nullptr;
  }
}

void CallbackInvoker::Finish() {
  lock_guard<mutex>  guard(args_mutex_);
  if (to_call_) {

// TODO(Art): temp
cout << "CallbackInvoker Invoking in JS thread\n";

    Invoke(cb_func_, args_);
  }
  CleanUp();
}

void CallbackInvoker::Invoke(PersistentFunctionCopyable fn, const vector<PersistentValueCopyable>& args) {
  auto isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  vector<Local<Value>> call_args;
  call_args.reserve(args.size());
  for (const auto& a : args) {
    call_args.emplace_back(a.Get(isolate));
  }
  // node::MakeCallback(isolate, Object::New(isolate), 
  //                    fn.Get(isolate), static_cast<int>(call_args.size()), call_args.data());

  fn.Get(isolate)->Call(isolate->GetCurrentContext()->Global(), static_cast<int>(call_args.size()), call_args.data());
}

void CallbackInvoker::CleanUp() {
  cb_func_.Reset();
  for (auto& a : args_) {
    a.Reset();
  }
  args_.clear();
}

// EventEmitter

EventEmitter::EventEmitter()
  : worker_(BlockingWorker::New()) {
}

void EventEmitter::Start() {
// TODO(Art): temp
cout << "Emitter starting worker\n";
  worker_->StartWork();
}

void EventEmitter::AddListener(PersistentFunctionCopyable cb_func) {
  lock_guard<mutex>  guard(mutex_);
  listeners_.emplace_back(cb_func);

// TODO(Art): temp
cout << "Emitter listener added\n";
}

void EventEmitter::AddListener(v8::Local<v8::Function> cb_func) {
  AddListener(PersistentFunctionCopyable(Isolate::GetCurrent(), cb_func));
}

void EventEmitter::Stop() {
  worker_->StopWork();  // it suicides
  worker_ = nullptr;
}

EventEmitter::~EventEmitter() {
// TODO(Art): temp
cout << "~EventEmitter\n";
  if (worker_) {
    worker_->StopWork();
  }
  for (auto& lis : listeners_) {
    lis.Reset();
  }
}

namespace {
// payload data
struct EventEmission {
  vector<PersistentFunctionCopyable> listeners;
  vector<PersistentValueCopyable> args;
  uv_async_t async;
};
}  // anonymous namespace

void EventEmitter::Emit(const vector<v8::Local<v8::Value>>& args) {
// TODO(Art): temp
cout << "Emitting\n";

  auto e = new EventEmission;
  { lock_guard<mutex>  guard(mutex_);
    e->listeners = listeners_;
  }
  auto isolate = Isolate::GetCurrent();
  e->args.reserve(args.size());
  for (const auto& a : args) {
    e->args.emplace_back(isolate, a);
  }
  uv_async_init(uv_default_loop(), &e->async, EmitEvent);
  e->async.data = e;
  auto r = uv_async_send(&e->async);
  if (0 != r) {
    // what to do?
  }

// TODO(Art): temp
cout << "Emitter async_send " << r << "\n";
}

void EventEmitter::EmitEvent(uv_async_t* handle) {
// TODO(Art): temp
cout << "Emitter EmitEvent\n";

  auto e = static_cast<EventEmission*>(handle->data);
  assert(e);

  for (auto& lis : e->listeners) {
    CallbackInvoker::Invoke(lis, e->args);
  }

// TODO(Art): listener.Reset()

  delete e;
}

}  // namespace node_addon
}  // namespace at
