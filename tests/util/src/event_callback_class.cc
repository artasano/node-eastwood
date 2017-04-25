/// @copyright © 2017 Airtime Media.  All rights reserved.

// TODO(Art): temp
#include <iostream>

#include <thread>
#include "event_callback_class.h"
#include "util/addon_util.h"

namespace addon_test {

using v8::FunctionCallback;
using v8::Value;
using v8::Undefined;

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

  self->event1_.Emit(ToString(args[0]) + " event1");
}

void EventCallbackClass::fireEvent2InWorkerThread(const FunctionCallbackInfo<Value>& args) {
  if (!CheckArgs("fireEvent1InJSThread", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsString(); }
  )) return;

  EventCallbackClass* self = Unwrap<EventCallbackClass>(args.Holder());
  assert(self);

  auto msg = ToString(args[0]);

// TODO(Art): temp
std::cout << "firing event 2 " << msg << std::endl;

  auto t = thread([self, msg]() {

// TODO(Art): temp
std::cout << "firing event 2 in thread " << msg << std::endl;

    self->event2_.Emit(msg + " event2");
  });
  t.detach();
}

void EventCallbackClass::callbackInJSThread(const FunctionCallbackInfo<Value>& args) {
  if (!CheckArgs("callbackInJSThread", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsInt32(); }
  )) return;

  EventCallbackClass* self = Unwrap<EventCallbackClass>(args.Holder());
  assert(self);

  self->callback_.Call(nullptr, ToInt32(args[0]) + 1);
}

void EventCallbackClass::callbackInWorkerThread(const FunctionCallbackInfo<Value>& args) {
  if (!CheckArgs("callbackInWorkerThread", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsInt32(); }
  )) return;

  EventCallbackClass* self = Unwrap<EventCallbackClass>(args.Holder());
  assert(self);

  auto a0 = ToInt32(args[0]);
  thread t([self, a0]() {
    self->callback_.Call(nullptr, a0 + 1);
  });
  t.detach();
}

void EventCallbackClass::callbackErrorInWorkerThread(const FunctionCallbackInfo<Value>& args) {
  if (!CheckArgs("callbackErrorInWorkerThread", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsString(); }
  )) return;

  EventCallbackClass* self = Unwrap<EventCallbackClass>(args.Holder());
  assert(self);

  auto msg = ToString(args[0]);
  auto t = thread([self, msg]() mutable {
    self->callback_.Call(at::node_addon::V8TypeError(msg + " exception"));
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
