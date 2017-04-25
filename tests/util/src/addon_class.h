/// @copyright © 2017 Airtime Media.  All rights reserved.

#ifndef ADDON_CLASS_H_
#define ADDON_CLASS_H_

#include <node.h>
#include <node_object_wrap.h>
#include "util/addon_util.h"

namespace addon_test {

class AddonClass : public node::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);

  enum { Enum1, Enum2 };
  static constexpr auto IntConstant1 = 10;
  static constexpr auto UintConstant1 = 11u;
  static constexpr auto FloatConstant1 = 1.1f;
  static constexpr auto DoubleConstant1 = 1.2;
  static constexpr auto BoolConstantT = true;
  static constexpr auto BoolConstantF = false;
  static constexpr auto CharConstant1 = 'x';
  static constexpr auto StringConstant1 = "abc";

 private:
  // bool()
  static void testConversionToFromV8(const v8::FunctionCallbackInfo<v8::Value>& args);
  // bool()
  static void testConversionFailure(const v8::FunctionCallbackInfo<v8::Value>& args);
  // void()
  static void testCheckArgs0(const v8::FunctionCallbackInfo<v8::Value>& args);
  // void(function)
  static void testCheckArgs0or1(const v8::FunctionCallbackInfo<v8::Value>& args);
  // double(double, uint32_t), the second arg must be less than 10
  static void testCheckArgs2(const v8::FunctionCallbackInfo<v8::Value>& args);

  AddonClass() = default;
  ~AddonClass() = default;

  /// @internal called by V8 framewodk
  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;

  AT_ADDON_CLASS;
};

}  // namespace addon_test

#endif  // ADDON_CLASS_H_
