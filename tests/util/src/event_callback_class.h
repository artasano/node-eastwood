/// @copyright © 2017 Airtime Media.  All rights reserved.

#ifndef EVENT_CALLBACK_CLASS_H_
#define EVENT_CALLBACK_CLASS_H_

#include <node.h>
#include <node_object_wrap.h>

#include "util/addon_util.h"


namespace addon_test {

class EventCallbackClass : public node::ObjectWrap {
 public:
  /// @internal Used for V8 framework
  static void Init(v8::Local<v8::Object> exports);

 private:
  // public interface

  // void("eveit1"|"event2", cb)
  //  cb: function(string);
  static void on(const v8::FunctionCallbackInfo<v8::Value>& args);

  // void(cb)
  //  cb: function(err, int)
  static void runAsync(const v8::FunctionCallbackInfo<v8::Value>& args);

  // void()
  static void stop(const v8::FunctionCallbackInfo<v8::Value>& args);;

  // test method void(string) : return arg + " event";
  static void fireEvent1InJSThread(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void fireEvent2InWorkerThread(const v8::FunctionCallbackInfo<v8::Value>& args);

  // test method void(int) : return arg + 1
  static void callbackInJSThread(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void callbackInWorkerThread(const v8::FunctionCallbackInfo<v8::Value>& args);
  // void(string) : throw arg + " excepton"
  static void callbackErrorInWorkerThread(const v8::FunctionCallbackInfo<v8::Value>& args);

 private:
  EventCallbackClass() = default;
  ~EventCallbackClass() = default;

  at::node_addon::EventEmitter event1_;
  at::node_addon::EventEmitter event2_;
  at::node_addon::CallbackInvoker callback_;

  /// @internal Used by V8 framework
  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  /// @internal Used by V8 framework
  static v8::Persistent<v8::Function> constructor;

  AT_ADDON_CLASS;
};

}  // namespace addon_test

#endif  // EVENT_CALLBACK_CLASS_H_
