/// @copyright © 2017 Airtime Media.  All rights reserved.

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

  /// Enums are converted between C++ and V8 in this type
  using EnumType = int32_t;
  using V8EnumType = v8::Int32;

  /// Checks the type
  static bool IsV8EnumType(const v8::Local<v8::Value>& val);

  /// Gets the value @throw if @a val type is not V8EnumType
  static EnumType ToEnumType(v8::Isolate* isolate, const v8::Local<v8::Value>& val);

  /// Creates enum spec for InitClass. Usefill if enum name is unqualified
  #define AT_ADDON_CLASS_ENUM(ENUM) at::node_addon::Util::Enum(#ENUM, ENUM)

  /// Creates enum spec for InitClass. Use this version (instead of macro) if enum name has qualifier.
  static std::pair<std::string, EnumType> Enum(const std::string& name, EnumType val);

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

  /**
   * Creates new Cpp instance.
   */
  template <class T>
  static bool NewCppInstance(const v8::FunctionCallbackInfo<v8::Value>& args, T* obj);

  /**
   * Checks function arguments given by V8.
   * @param checks check function bool(const Local<Value> arg_val, string& err_msg)
   *        that returns false if check fails.
   *        arg_val [in]: Nth argument
   *        err_msg [out]: error message (used only when returned false. default is used if not set)
   *        called sequentially beginning with args[0]
   * @return false if check failed. JS exception has been thrown in that case.
   */
  template <class... CheckFunc>
  static bool CheckArgs(
          v8::Isolate* isolate, const std::string& context, 
          const v8::FunctionCallbackInfo<v8::Value>& args,
          size_t min_num_args, size_t max_num_args,
          CheckFunc&&... checks);
};

}  // namespace node_addon
}  // namespace at

#include "addon_util.hpp"

#endif  // AT_ADDON_UTIL_H_
