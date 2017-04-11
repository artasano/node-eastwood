#ifndef EASTWOOD_H_
#define EASTWOOD_H_

#include <node.h>
#include <node_object_wrap.h>

#include "mediacore/defs.h"
#include "mediacore/async/eventloop.h"

namespace ew {

class Subscriber : public node::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);

  /**
   * @param init value
   */
  static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);

 private:
  explicit Subscriber(double value = 0);
  ~Subscriber();

  /**
   * @param value to add
   * @cb called with subscriber
   * @return result value
   */
  static void Add(const v8::FunctionCallbackInfo<v8::Value>& args);

  /// @return value
  static void GetValue(const v8::FunctionCallbackInfo<v8::Value>& args);

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;

  double value_;

  friend class EastWood;
};

class EastWood : public node::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);

 private:
  explicit EastWood();
  ~EastWood();

  /**
   * @param init value
   */
  static void CreateSubscriber(const v8::FunctionCallbackInfo<v8::Value>& args);

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;
};

}  // namespace ew

#endif  // EASTWOOD_H_

