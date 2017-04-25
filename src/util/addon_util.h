/// @copyright © 2017 Airtime Media.  All rights reserved.

#ifndef AT_ADDON_UTIL_H_
#define AT_ADDON_UTIL_H_

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <future>
#include <mutex>
#include <atomic>
#include <tuple>

#include <node.h>
#include <uv.h>

namespace at {
namespace node_addon {

using PersistentFunctionCopyable = v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>;
using PersistentValueCopyable = v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>>;

/// Converts V8 String to std::string
std::string ToString(const v8::Local<v8::Value> v8str);

/// Inspects V8 value
std::string Inspect(v8::Local<v8::Value> value);

/// Throws an Exception
void ThrowException(
                  v8::Local<v8::Value>(* ex)(v8::Local<v8::String>), const std::string& msg);

/// Throws an Exception with class name
void ThrowException(
                  const v8::FunctionCallbackInfo<v8::Value>& args,
                  v8::Local<v8::Value>(* ex)(v8::Local<v8::String>),
                  const std::string& msg);

/// Converts V8 Int32 to int32_t @throw std::logic_error if @a val is not convertible to int32_t
int32_t ToInt32(v8::Local<v8::Value> val);

/// Converts V8 Uint32 to uint32_t @throw std::logic_error if @a val is not convertible to uint32_t
uint32_t ToUint32(v8::Local<v8::Value> val);

/// Converts V8 Number to double @throw std::logic_error if @a val is not convertible to double
double ToDouble(v8::Local<v8::Value> val);

/// Converts V8 Bool to bool
bool ToBool(v8::Local<v8::Value> val);

// ToLocal family - caller needs to hold HandleScope. Otherwise runtime error occurs.

/// Converts C++ bool to v8::Local<v8::Value>
v8::Local<v8::Value> ToLocalValue(bool value);

/// Converts C++ int32_t to v8::Local<v8::Value>
v8::Local<v8::Value> ToLocalValue(int32_t value);

/// Converts C++ uint32_t to v8::Local<v8::Value>
v8::Local<v8::Value> ToLocalValue(uint32_t value);

/// Converts C++ int64_t to v8::Local<v8::Value> (trims to int32_t)
v8::Local<v8::Value> ToLocalValue(int64_t value);

/// Converts C++ uint64_t to v8::Local<v8::Value> (trims to uint32_t)
v8::Local<v8::Value> ToLocalValue(uint64_t value);

/// Converts C++ double to v8::Local<v8::Value>
v8::Local<v8::Value> ToLocalValue(double value);

/// Converts C++ string to v8::Local<v8::Value>
v8::Local<v8::Value> ToLocalValue(const std::string& value);

/// Converts C++ bool to v8::Local<v8::Boolean>
v8::Local<v8::Boolean> ToLocalBoolean(bool value);

/// Converts C++ int32_t to v8::Local<v8::Integer>
v8::Local<v8::Integer> ToLocalInteger(int32_t value);

/// Converts C++ uint32_t to v8::Local<v8::Integer>
v8::Local<v8::Integer> ToLocalInteger(uint32_t value);

/// Converts C++ int64_t to v8::Local<v8::Integer> (trims to int32_t)
v8::Local<v8::Integer> ToLocalInteger(int64_t value);

/// Converts C++ uint64_t to v8::Local<v8::Integer> (trims to uint32_t)
v8::Local<v8::Integer> ToLocalInteger(uint64_t value);

/// Converts C++ double to v8::Local<v8::Number>
v8::Local<v8::Number> ToLocalNumber(double value);

/// Converts C++ string to v8::Local<v8::String>
v8::Local<v8::String> ToLocalString(const std::string& value);

// InitClass family

/// Creates prototype spec for InitClass. Usefull if C++ method name is exported as is.
#define AT_ADDON_PROTOTYPE_METHOD(METHOD) at::node_addon::Prototype(#METHOD, METHOD)

/// Creates prototype spec for InitClass. Use this version (instead of macro) if C++ method name is different.
std::pair<std::string, v8::FunctionCallback> Prototype(const std::string& name, v8::FunctionCallback func);

/// Creates constant spec for InitClass. Usefill if the constant name is unqualified
#define AT_ADDON_CLASS_CONSTANT(CONST) at::node_addon::Constant(#CONST, CONST)

/// Creates class constant spec for InitClass. Use this version (instead of macro) if the name has qualifier.
template <typename T>
std::pair<std::string, T> Constant(const std::string& name, T&& val);

/// Initializes a class
template <class... Prop>  // Prototype(..) or Enum(..)
void InitClass(
          v8::Local<v8::Object> exports, const std::string& class_name,
          void(* new_func)(const v8::FunctionCallbackInfo<v8::Value>&),
          v8::Persistent<v8::Function>& constructor,
          Prop&&... props);

/// Creates new V8 instance
v8::Local<v8::Object> NewV8Instance(
          v8::Persistent<v8::Function>& constructor, const v8::FunctionCallbackInfo<v8::Value>& args);

/**
  * Creates new Cpp instance.
  * The class T definition needs to include AT_ADDON_CLASS. (It can be in private section)
  */
template <class T>
bool NewCppInstance(const v8::FunctionCallbackInfo<v8::Value>& args, T* obj);

/// Helper to make NewCppInstance work (Need this because Wrap() is ObjectWrap's protected member, and NewCppInstance needs to use it.)
#define AT_ADDON_CLASS template <class T> friend bool at::node_addon::NewCppInstance(const v8::FunctionCallbackInfo<v8::Value>&, T*)

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
bool CheckArgs(
        const std::string& context, 
        const v8::FunctionCallbackInfo<v8::Value>& args,
        size_t min_num_args, size_t max_num_args,
        CheckFunc&&... checks);



/// v8 Exception counterpart to feed into CallbackInvoker::Call and EventEmitter::Emit
struct V8Exception {
  explicit V8Exception(const std::string& msg) : msg_(msg) {}
  std::string msg_;
  virtual v8::Local<v8::Value> ToV8() const = 0;
};
struct V8RangeError : public V8Exception {
  explicit V8RangeError(const std::string& msg) : V8Exception(msg) {}
  v8::Local<v8::Value> ToV8() const override { return v8::Exception::RangeError(ToLocalString(msg_)); }
};
struct V8ReferenceError : public V8Exception {
  explicit V8ReferenceError(const std::string& msg) : V8Exception(msg) {}
  v8::Local<v8::Value> ToV8() const override { return v8::Exception::ReferenceError(ToLocalString(msg_)); }
};
struct V8SyntaxError : public V8Exception {
  explicit V8SyntaxError(const std::string& msg) : V8Exception(msg) {}
  v8::Local<v8::Value> ToV8() const override { return v8::Exception::SyntaxError(ToLocalString(msg_)); }
};
struct V8TypeError : public V8Exception {
  explicit V8TypeError(const std::string& msg) : V8Exception(msg) {}
  v8::Local<v8::Value> ToV8() const override { return v8::Exception::TypeError(ToLocalString(msg_)); }
};
struct V8Error : public V8Exception {
  explicit V8Error(const std::string& msg) : V8Exception(msg) {}
  v8::Local<v8::Value> ToV8() const override { return v8::Exception::Error(ToLocalString(msg_)); }
};

v8::Local<v8::Value> ToLocalValue(const V8Exception& ex);


/**
 * A libuv worker class used by CallbackInvoker and EventEmitter (but generic enough for public use)
 * The responsibility of this class is to hold uv_work in the uv event loop,
 * so Node JS program does not exit until this class is destroyed.
 */
class BlockingWorker {
  BlockingWorker();
  ~BlockingWorker();

 public:
  /**
   * Constructs. This needs to be called in Node JS thread. This ctor does not queue uv_work until StartWork().
   * Should be created by this method and held by the pointer. Do not access after StopWork();
   */
  static BlockingWorker* New();

   /**
    * This needs to be called in Node JS thread. This queues uv_work. If called again, this does nothing.
    * @param after_fn: called when StopWork() is called.
    */
  void StartWork(std::function<void()> after_fn = std::function<void()>());

  /**
   * This finishes the uv work. It can be called in any thread. It deletes this object.
   * If the user forgot to call StopWork(), Node JS process would keep running.
   */
  void StopWork();

 private:
  uv_work_t work_;
  std::promise<bool> promise_;
  bool queued_ = false;
  std::function<void()> after_fn_;

  static void KeepAliveUntilFuture(uv_work_t* work);
  static void Finish(uv_work_t* work, int status);
};



/// A non-template base class of CallbackInvoker
class CallbackInvokerBase {
 public:
   /// Constructs. This needs to be used in Node JS thread. This ctor does not queue uv_work until callback func is set.
  CallbackInvokerBase();

   /// Constructs. This needs to be used in Node JS thread. This ctor queues uv_work immediately.
  explicit CallbackInvokerBase(PersistentFunctionCopyable cb_func);
  explicit CallbackInvokerBase(v8::Local<v8::Function> cb_func);

  /// If callback function was set in the constructor, or previoud SetCallbackFn call, it'll be replaced.
  void SetCallbackFn(PersistentFunctionCopyable cb_func);
  void SetCallbackFn(v8::Local<v8::Function> cb_func);

  /// Finishes the uv worker
  void Stop();

  ~CallbackInvokerBase();

  /// utility function exposed
  static void Invoke(PersistentFunctionCopyable fn, const std::vector<v8::Local<v8::Value>>& args);
  static void Invoke(PersistentFunctionCopyable fn, std::vector<v8::Local<v8::Value>>&& args);

 protected:
  bool to_call() const { return to_call_; }
  std::atomic<bool>& to_call() { return to_call_; }
  void FinishWorker();
  PersistentFunctionCopyable& cb_func() { return cb_func_; }

 private:
  BlockingWorker* worker_ = nullptr;
  PersistentFunctionCopyable cb_func_;
  std::atomic<bool> to_call_;

  void Finish();
  virtual void FinishImpl() {};
  void CleanUp();
};


/**
 * A helper class to hold a Node JS callback function object and call it from any thread.
 * This class holds uv_work in the uv event loop, so Node JS program does not exit until
 * the callback is called (or this class is destroyed).
 */
template <typename ... Args>
class CallbackInvoker : public CallbackInvokerBase {
 public:
   /// Constructs. This needs to be used in Node JS thread. This ctor does not queue uv_work until callback func is set.
  CallbackInvoker();

  ~CallbackInvoker();

   /// Constructs. This needs to be used in Node JS thread. This ctor queues uv_work immediately.
  explicit CallbackInvoker(PersistentFunctionCopyable cb_func);
  explicit CallbackInvoker(v8::Local<v8::Function> cb_func);

  /**
   * Sends a call. It can be called in any thread.
   * This removes uv_work from the event loop, so if no other pending task remains, the program ends.
   * @note This can be called only once. If called twice or more, the later calls are ignored.
   */
  template <typename ... CallArgs>
  void Call(CallArgs&&... args);

 private:
  std::tuple<std::shared_ptr<Args>...> args_;

  void FinishImpl() override;
};



/// Non-template base class of EventEmitter
class EventEmitterBase {
 public:
   /// Constructs. This needs to be used in Node JS thread. Need to call Start() to queue uv_work.
  EventEmitterBase();

  /// Adds listener (there is no way to remove listener, for now)
  void AddListener(PersistentFunctionCopyable cb_func);
  void AddListener(v8::Local<v8::Function> cb_func);

  /// Queues uv_work so that event emission becomes available.
  void Start();

  /// Finishes the uv worker
  void Stop();

  ~EventEmitterBase();

 protected:
  std::vector<PersistentFunctionCopyable> CopyListeners();

 private:
  BlockingWorker* worker_ = nullptr;
  std::mutex mutex_;
  std::vector<PersistentFunctionCopyable> listeners_;

  void Finish();
  virtual void FinishImpl() {};
  void CleanUp();
};


/**
 * A helper class to hold a Node JS callback function objects and call it from any thread.
 * This class holds uv_work in the uv event loop, so Node JS program does not exit until this class is destroyed.
 */
template <typename ... Args>
class EventEmitter : public EventEmitterBase {
 public:
   /// Constructs. This needs to be used in Node JS thread. Need to call Start() to queue uv_work.
  EventEmitter();
  ~EventEmitter();

  /**
   * Emits an event to all listeners. It can be called in any thread.
   */
  template <typename ... EmitArgs>
  void Emit(EmitArgs&&... args);

 private:
  static void EmitEvent(uv_async_t* handle);
};

}  // namespace node_addon
}  // namespace at

#include "addon_util.hpp"

#endif  // AT_ADDON_UTIL_H_
