/// Copyright Airtime Media, Inc. 2017

#ifndef AT_ADDON_UTIL_H_
#define AT_ADDON_UTIL_H_

#include <string>
#include <utility>

#include <node.h>

namespace at {
namespace node_addon {

struct Util {
  /// Converts V8 String to std::string
  static std::string ToString(const v8::Local<v8::Value> v8str);

  /// Inspects V8 value
  static std::string Inspect(v8::Isolate* isolate, v8::Local<v8::Value> value);

  /// Throws an Exception
  static void ThrowException(v8::Isolate* isolate,
                    v8::Local<v8::Value>(* ex)(v8::Local<v8::String>), const std::string& msg);

  /// Throws an Exception with class name
  static void ThrowException(
                    const v8::FunctionCallbackInfo<v8::Value>& args,
                    v8::Local<v8::Value>(* ex)(v8::Local<v8::String>),
                    const std::string& msg);

  /// Converts V8 Int32 to int32_t @throw std::logic_error if @a val is not convertible to int32_t
  static int32_t ToInt32(v8::Isolate* isolate, v8::Local<v8::Value> val);

  /// Converts V8 Uint32 to uint32_t @throw std::logic_error if @a val is not convertible to uint32_t
  static uint32_t ToUint32(v8::Isolate* isolate, v8::Local<v8::Value> val);

  /// Converts V8 Number to double @throw std::logic_error if @a val is not convertible to double
  static double ToDouble(v8::Isolate* isolate, v8::Local<v8::Value> val);

  /// Converts V8 Bool to bool
  static bool ToBool(v8::Isolate* isolate, v8::Local<v8::Value> val);

  // InitClass family

  /// Creates prototype spec for InitClass
  static std::pair<std::string, v8::FunctionCallback> Prototype(const std::string& name, v8::FunctionCallback func);

  /// Creates enum spec for InitClass. Usefill if enum name is unqualified
  #define AT_ADDON_CLASS_ENUM(ENUM) at::node_addon::Util::Enum(#ENUM, ENUM)

  /// Creates enum spec for InitClass. Use this version (instead of macro) if enum name has qualifier.
  static std::pair<std::string, int> Enum(const std::string& name, int val);

  /// Initializes a class
  template <class... Prop>  // Prototype(..) or Enum(..)
  static void InitClass(
            v8::Local<v8::Object> exports, const std::string& class_name,
            void(* new_func)(const v8::FunctionCallbackInfo<v8::Value>&),
            v8::Persistent<v8::Function>& constructor,
            Prop&&... props);

  /// Creates new V8 instance
  static v8::Local<v8::Object> NewV8Instance(
            v8::Persistent<v8::Function>& constructor, const v8::FunctionCallbackInfo<v8::Value>& args);

  /// Creates new Cpp instance. Using macro because Wrap is a protected method in ObjectWrap base class
  #define AT_ADDON_NEW_CPP_INSTANCE(V8_ARGS, CLASS, CTOR_ARGS) \
    at::node_addon::Util::NewCppInstance(V8_ARGS, #CLASS, [&V8_ARGS]() { \
      auto obj = new CLASS(CTOR_ARGS); \
      obj->Wrap(V8_ARGS.This()); \
      return V8_ARGS.This(); \
    })

  /// @internal used by AT_ADDON_NEW_CPP_INSTANCE
  static void NewCppInstance(
                const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& class_name,
                std::function<v8::Local<v8::Object>()> create_and_wrap_func);

  /// Checks function arguments given by V8
  template <class... CheckFunc> // CheckFunc: bool(const Local<Value>)
  static void CheckArgs(
          v8::Isolate* isolate, const std::string& context, 
          const v8::FunctionCallbackInfo<v8::Value>& args, size_t num_args,
          CheckFunc&&... checks);
};

}  // namespace node_addon
}  // namespace at

#include "addon_util.hpp"

#endif  // AT_ADDON_UTIL_H_
