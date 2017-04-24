/// @copyright © 2017 Airtime Media.  All rights reserved.

#include "addon_class.h"
#include "util/addon_util.h"

namespace addon_test {

// using v8::Context;
using v8::Function;
// using v8::FunctionCallbackInfo;
// using v8::FunctionCallback;
// using v8::FunctionTemplate;
// using v8::Isolate;
// using v8::Local;
// using v8::Number;
// using v8::Uint32;
// using v8::Int32;
// using v8::Boolean;
// using v8::Object;
using v8::Persistent;
// using v8::String;
// using v8::Symbol;
// using v8::Value;
// using v8::Exception;
// using v8::PropertyAttribute;

using namespace std;
using namespace string_literals;
using namespace chrono_literals;

using namespace at::node_addon;

Persistent<Function> AddonClass::constructor;

void AddonClass::Init(Local<Object> exports) {
  InitClass(exports, "AddonClass", New, constructor,
    AT_ADDON_PROTOTYPE_METHOD(testConversionToFromV8),
    AT_ADDON_PROTOTYPE_METHOD(testCheckArgs0),
    AT_ADDON_PROTOTYPE_METHOD(testCheckArgs0or1),
    AT_ADDON_PROTOTYPE_METHOD(testCheckArgs2),

    AT_ADDON_CLASS_CONSTANT(Enum1),
    AT_ADDON_CLASS_CONSTANT(Enum2),
    AT_ADDON_CLASS_CONSTANT(IntConstant1),
    AT_ADDON_CLASS_CONSTANT(UintConstant1),
    AT_ADDON_CLASS_CONSTANT(FloatConstant1),
    AT_ADDON_CLASS_CONSTANT(DoubleConstant1),
    AT_ADDON_CLASS_CONSTANT(BoolConstantT),
    AT_ADDON_CLASS_CONSTANT(BoolConstantF),
    AT_ADDON_CLASS_CONSTANT(CharConstant1),
    AT_ADDON_CLASS_CONSTANT(StringConstant1)
  );
}

void AddonClass::testConversionToFromV8(const FunctionCallbackInfo<Value>& args) {
  args.GetReturnValue().Set(ToLocalBoolean(false));

  auto t = true;
  auto local_true = ToLocalBoolean(t);
  if (!ToBool(local_true)) {
    ThrowException(args, Exception::TypeError, "bool true");
    return;
  }
  auto f = false;
  auto local_false = ToLocalBoolean(f);
  if (ToBool(local_false)) {
    ThrowException(args, Exception::TypeError, "bool false");
    return;
  }
  auto i = -123;
  auto local_int = ToLocalInteger(i);
  if (-123 != ToInt32(local_int)) {
    ThrowException(args, Exception::TypeError, "int");
    return;
  }
  auto u = 456u;
  auto local_uint = ToLocalInteger(u);
  if (456 != ToUint32(local_uint)) {
    ThrowException(args, Exception::TypeError, "uint");
    return;
  }
  auto d = 12.3;
  auto local_num = ToLocalNumber(d);
  if (12.3 != ToDouble(local_num)) {
    ThrowException(args, Exception::TypeError, "double");
    return;
  }
  auto s = "abc"s;
  auto local_str = ToLocalString(s);
  if ("abc" != ToString(local_str)) {
    ThrowException(args, Exception::TypeError, "string");
    return;
  }
  args.GetReturnValue().Set(ToLocalBoolean(true));
}

void AddonClass::testCheckArgs0(const FunctionCallbackInfo<Value>& args) {
  if (!CheckArgs("testCheckArgs0", args, 0, 0)) return;
}

void AddonClass::testCheckArgs0or1(const FunctionCallbackInfo<Value>& args) {
  if (!CheckArgs("testCheckArgs0or1", args, 0, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsFunction(); }
  )) return;
}

void AddonClass::testCheckArgs2(const FunctionCallbackInfo<Value>& args) {
  if (!CheckArgs("testCheckArgs2", args, 2, 2,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsNumber(); },
    [](const Local<Value> arg1, string& err_msg) { return arg1->IsUint32(); }
  )) return;
  args.GetReturnValue().Set(ToLocalNumber(ToDouble(args[0]) + ToUint32(args[1])));
}

void AddonClass::New(const FunctionCallbackInfo<Value>& args) {
  NewCppInstance<AddonClass>(args, new AddonClass());
}

}  // namespace addon_test
