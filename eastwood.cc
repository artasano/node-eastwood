#include "eastwood.h"

namespace ew {

using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Value;
using v8::Exception;

Persistent<Function> Subscriber::constructor;

Subscriber::Subscriber(double value) : value_(value) {
}

Subscriber::~Subscriber() {
}

void Subscriber::Init(Local<Object> exports) {
  HandleScope scope;

  // Prepare constructor template
  auto tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "Subscriber"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  NODE_SET_PROTOTYPE_METHOD(tpl, "add", Add);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getValue", GetValue);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "Subscriber"),
               tpl->GetFunction());
}

void Subscriber::New(const FunctionCallbackInfo<Value>& args) {
  HandleScope scope;

  if (args.IsConstructCall()) {
    // Check the number of arguments passed.
    if (args.Length() < 1) {
      // Throw an Error that is passed back to JavaScript
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Wrong number of arguments")));
      return;
    }

    // Check the argument types
    if (!args[0]->IsNumber()) {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Wrong arguments")));
      return;
    }
    // construct
    double value = args[0]->NumberValue();
    auto obj = new Subscriber(value);
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    scope.GetIsolate()->ThrowException(Exception::SyntaxError(
        String::NewFromUtf8(isolate, "Use 'new' to instantiate EastWood")));
    return;
  }
}

void Subscriber::NewInstance(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  // arg checks are done in ctor
  const unsigned argc = 1;
  Local<Value> argv[argc] = { args[0] };
  Local<Function> cons = Local<Function>::New(isolate, constructor);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> instance =
      cons->NewInstance(context, argc, argv).ToLocalChecked();

  args.GetReturnValue().Set(instance);
}

// double(double, cb), cb: void(Subscriber);
void Subscriber::Add(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  // Check the number of arguments passed.
  if (args.Length() < 2) {
    // Throw an Error that is passed back to JavaScript
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  // Check the argument types
  if (!args[0]->IsNumber() || !args[1]->IsFunction()) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong arguments")));
    return;
  }

  // perform the operation
  auto obj = ObjectWrap::Unwrap<Subscriber>(args.Holder());
  double value = args[0]->NumberValue();
  obj->value_ += value;

  // set the return value
  args.GetReturnValue().Set(Number::New(isolate, obj->value_));

  // also callback
  auto cb = Local<Function>::Cast(args[1]);
  const unsigned argc = 1;
  Local<Value> argv[argc] = { args.Holder() };
  cb->Call(Null(isolate), argc, argv);
}

void Subscriber::GetValue(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  // set the return value
  Subscriber* obj = ObjectWrap::Unwrap<Subscriber>(args.Holder());
  args.GetReturnValue().Set(Number::New(isolate, obj->value_));
}

// --------------------------------------------

Persistent<Function> EastWood::constructor;

EastWood::EastWood() {
}

EastWood::~EastWood() {
}

void EastWood::Init(Local<Object> exports) {
  auto isolate = exports->GetIsolate();

  // Prepare constructor template
  auto tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "EastWood"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  NODE_SET_PROTOTYPE_METHOD(tpl, "createSubscriber", CreateSubscriber);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "EastWood"),
               tpl->GetFunction());
}

void EastWood::New(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new EastWood(...)`
    auto obj = new EastWood();
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    isolate->ThrowException(Exception::SyntaxError(
        String::NewFromUtf8(isolate, "Use 'new' to instantiate EastWood")));
    return;

  }
}

// Subscriber(double)
void EastWood::CreateSubscriber(const v8::FunctionCallbackInfo<v8::Value>& args) {
  auto isolate = args.GetIsolate();

  // Arg checks are done in Subscriber ctor
  const int argc = 1;
  Local<Value> argv[argc] = { args[0] };
  auto cons = Local<Function>::New(isolate, Subscriber::constructor);
  auto context = isolate->GetCurrentContext();
  auto instance = cons->NewInstance(context, argc, argv).ToLocalChecked();
  args.GetReturnValue().Set(instance);
}

}  // namespace ew
