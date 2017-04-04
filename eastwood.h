#ifndef EASTWOOD_H_
#define EASTWOOD_H_

#include <node.h>
#include <node_object_wrap.h>

namespace ew {

class EastWood : public node::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);

 private:
  explicit EastWood(double value = 0);
  ~EastWood();

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Add(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;
  double value_;
};

}  // namespace ew

#endif  // EASTWOOD_H_

