/// @copyright © 2017 Airtime Media.  All rights reserved.

#include <thread>
#include "event_callback_class.h"
#include "util/addon_util.h"

namespace addon_test {

// using Context;
// using Function;
// using FunctionCallbackInfo;
using v8::FunctionCallback;
// using FunctionTemplate;
// using Isolate;
// using Local;
// using Number;
// using Uint32;
// using Int32;
// using Boolean;
// using Object;
// using Persistent;
// using String;
// using Symbol;
using v8::Value;
using v8::Undefined;
// using Null;
// using Exception;
// using PropertyAttribute;

using namespace std;
using namespace string_literals;
using namespace chrono_literals;

using namespace at::node_addon;

Persistent<Function> EventCallbackClass::constructor;

void EventCallbackClass::on(const FunctionCallbackInfo<Value>& args) {
  string event;
  if (!CheckArgs("on", args, 2, 2,
    [&event](const Local<Value> arg0, string& err_msg) {
      if (!arg0->IsString()) return false;
      event = ToString(arg0);
      return ("event1" == event || "event2" == event);
    },
    [](const Local<Value> arg1, string& err_msg) { return arg1->IsFunction(); })) return;

  EventCallbackClass* self = Unwrap<EventCallbackClass>(args.Holder());
  assert(self);

  if ("event1" == event) {
    self->event1_.AddListener(Local<Function>::Cast(args[1]));
    self->event1_.Start();
  } else if ("event2" == event) {
    self->event2_.AddListener(Local<Function>::Cast(args[1]));
    self->event2_.Start();
  } else {
    // can't happen
    ThrowException(args, Exception::Error, "CheckArgs failed to check event type: got " + event);
    return;
  }
}

void EventCallbackClass::runAsync(const FunctionCallbackInfo<Value>& args) {
  if (!CheckArgs("runAsync", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsFunction(); }
  )) return;
  EventCallbackClass* self = Unwrap<EventCallbackClass>(args.Holder());
  assert(self);

  self->callback_.SetCallbackFn(Local<Function>::Cast(args[0]));
}

void EventCallbackClass::stop(const FunctionCallbackInfo<Value>& args) {
  if (!CheckArgs("stop", args, 0, 0)) return;
  EventCallbackClass* self = Unwrap<EventCallbackClass>(args.Holder());
  assert(self);

  self->event1_.Stop();
  self->event2_.Stop();
  self->callback_.Stop();
}

void EventCallbackClass::fireEvent1InJSThread(const FunctionCallbackInfo<Value>& args) {
  if (!CheckArgs("fireEvent1InJSThread", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsString(); }
  )) return;

  EventCallbackClass* self = Unwrap<EventCallbackClass>(args.Holder());
  assert(self);

  vector<Local<Value>> event_args;
  auto arg_s = ToString(args[0]);
  event_args.emplace_back(ToLocalValue(arg_s + " event"));
  self->event1_.Emit(event_args);
}

void EventCallbackClass::fireEvent2InWorkerThread(const FunctionCallbackInfo<Value>& args) {
  if (!CheckArgs("fireEvent1InJSThread", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsString(); }
  )) return;

  EventCallbackClass* self = Unwrap<EventCallbackClass>(args.Holder());
  assert(self);

  auto isolate = args.GetIsolate();
  PersistentValueCopyable arg_s(isolate, args[0]);
  auto t = thread([self, arg_s]() mutable {
    vector<Local<Value>> event_args;
    auto s = ToString(arg_s.Get(Isolate::GetCurrent()));
    event_args.emplace_back(ToLocalValue(s + " event"));
    self->event1_.Emit(event_args);
    arg_s.Reset();
  });
  t.detach();
}

void EventCallbackClass::callbackInJSThread(const FunctionCallbackInfo<Value>& args) {
  if (!CheckArgs("callbackInJSThread", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsInt32(); }
  )) return;

  EventCallbackClass* self = Unwrap<EventCallbackClass>(args.Holder());
  assert(self);

  vector<Local<Value>> callback_args;
  callback_args.emplace_back(Undefined(args.GetIsolate()));
  auto arg_i = ToInt32(args[0]);
  callback_args.emplace_back(ToLocalValue(arg_i + 1));
  self->callback_.Call(callback_args);
}
void EventCallbackClass::callbackInWorkerThread(const FunctionCallbackInfo<Value>& args) {
  if (!CheckArgs("callbackInWorkerThread", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsInt32(); }
  )) return;

  EventCallbackClass* self = Unwrap<EventCallbackClass>(args.Holder());
  assert(self);

  auto isolate = args.GetIsolate();
  PersistentValueCopyable arg_i(isolate, args[0]);
  thread t([self, arg_i]() mutable {
    vector<Local<Value>> callback_args;
    callback_args.emplace_back(Undefined(Isolate::GetCurrent()));
    auto i = ToInt32(arg_i.Get(Isolate::GetCurrent()));
    callback_args.emplace_back(ToLocalValue(i + 1));
    self->callback_.Call(callback_args);
    arg_i.Reset();
  });
  t.detach();
}

void EventCallbackClass::callbackErrorInWorkerThread(const FunctionCallbackInfo<Value>& args) {
  if (!CheckArgs("callbackErrorInWorkerThread", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsString(); }
  )) return;

  EventCallbackClass* self = Unwrap<EventCallbackClass>(args.Holder());
  assert(self);

  auto isolate = args.GetIsolate();
  PersistentValueCopyable arg_s(isolate, args[0]);
  auto t = thread([self, arg_s]() mutable {
    vector<Local<Value>> callback_args;
    auto s = ToString(arg_s.Get(Isolate::GetCurrent()));
    callback_args.emplace_back(Exception::Error(ToLocalString(s + " exception")));
    self->callback_.Call(callback_args);
    arg_s.Reset();
  });
  t.detach();
}

void EventCallbackClass::Init(Local<Object> exports) {
  InitClass(exports, "EventCallbackClass", New, constructor,
    AT_ADDON_PROTOTYPE_METHOD(on),
    AT_ADDON_PROTOTYPE_METHOD(runAsync),
    AT_ADDON_PROTOTYPE_METHOD(stop),
    AT_ADDON_PROTOTYPE_METHOD(fireEvent1InJSThread),
    AT_ADDON_PROTOTYPE_METHOD(fireEvent2InWorkerThread),
    AT_ADDON_PROTOTYPE_METHOD(callbackInJSThread),
    AT_ADDON_PROTOTYPE_METHOD(callbackInWorkerThread),
    AT_ADDON_PROTOTYPE_METHOD(callbackErrorInWorkerThread)
  );
}

void EventCallbackClass::New(const FunctionCallbackInfo<Value>& args) {
  NewCppInstance(args, new EventCallbackClass());
}

}  // namespace ew
