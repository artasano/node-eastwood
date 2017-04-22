/// @copyright © 2017 Airtime Media.  All rights reserved.

#ifndef AT_ADDON_UTIL_H_
#define AT_ADDON_UTIL_H_

#include <string>
#include <memory>
#include <vector>
#include <functional>

#include <node.h>
#include <uv.h>

#include "mediacore/defs.h"
#include "mediacore/async/promise.h"
#include "mediacore/thread/thread_annotations.h"

namespace at {
namespace node_addon {

using PersistentFunctionCopyable = v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>;
using PersistentValueCopyable = v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>>;

struct Util {
  /// Converts V8 String to std::string
  static std::string ToString(const v8::Local<v8::Value> v8str);

  /// Inspects V8 value
  static std::string Inspect(v8::Local<v8::Value> value);

  /// Throws an Exception
  static void ThrowException(
                    v8::Local<v8::Value>(* ex)(v8::Local<v8::String>), const std::string& msg);

  /// Throws an Exception with class name
  static void ThrowException(
                    const v8::FunctionCallbackInfo<v8::Value>& args,
                    v8::Local<v8::Value>(* ex)(v8::Local<v8::String>),
                    const std::string& msg);

  /// Converts V8 Int32 to int32_t @throw std::logic_error if @a val is not convertible to int32_t
  static int32_t ToInt32(v8::Local<v8::Value> val);

  /// Converts V8 Uint32 to uint32_t @throw std::logic_error if @a val is not convertible to uint32_t
  static uint32_t ToUint32(v8::Local<v8::Value> val);

  /// Converts V8 Number to double @throw std::logic_error if @a val is not convertible to double
  static double ToDouble(v8::Local<v8::Value> val);

  /// Converts V8 Bool to bool
  static bool ToBool(v8::Local<v8::Value> val);

  // ToLocal family

  /// Converts C++ bool to v8::Local<v8::Value>
  static v8::Local<v8::Value> ToLocalValue(bool value);

  /// Converts C++ int32_t to v8::Local<v8::Value>
  static v8::Local<v8::Value> ToLocalValue(int32_t value);

  /// Converts C++ uint32_t to v8::Local<v8::Value>
  static v8::Local<v8::Value> ToLocalValue(uint32_t value);

  /// Converts C++ int64_t to v8::Local<v8::Value> (trims to int32_t)
  static v8::Local<v8::Value> ToLocalValue(int64_t value);

  /// Converts C++ uint64_t to v8::Local<v8::Value> (trims to uint32_t)
  static v8::Local<v8::Value> ToLocalValue(uint64_t value);

  /// Converts C++ double to v8::Local<v8::Value>
  static v8::Local<v8::Value> ToLocalValue(double value);

  /// Converts C++ string to v8::Local<v8::Value>
  static v8::Local<v8::Value> ToLocalValue(const std::string& value);

  /// Converts C++ bool to v8::Local<v8::Boolean>
  static v8::Local<v8::Boolean> ToLocalBoolean(bool value);

  /// Converts C++ int32_t to v8::Local<v8::Integer>
  static v8::Local<v8::Integer> ToLocalInteger(int32_t value);

  /// Converts C++ uint32_t to v8::Local<v8::Integer>
  static v8::Local<v8::Integer> ToLocalInteger(uint32_t value);

  /// Converts C++ int64_t to v8::Local<v8::Integer> (trims to int32_t)
  static v8::Local<v8::Integer> ToLocalInteger(int64_t value);

  /// Converts C++ uint64_t to v8::Local<v8::Integer> (trims to uint32_t)
  static v8::Local<v8::Integer> ToLocalInteger(uint64_t value);

  /// Converts C++ double to v8::Local<v8::Number>
  static v8::Local<v8::Number> ToLocalNumber(double value);

  /// Converts C++ string to v8::Local<v8::String>
  static v8::Local<v8::String> ToLocalString(const std::string& value);

  // InitClass family

  /// Creates prototype spec for InitClass
  static std::pair<std::string, v8::FunctionCallback> Prototype(const std::string& name, v8::FunctionCallback func);

  /// Enums are converted between C++ and V8 in this type
  using EnumType = int32_t;
  using V8EnumType = v8::Int32;

  /// Checks the type
  static bool IsV8EnumType(const v8::Local<v8::Value>& val);

  /// Gets the value @throw if @a val type is not V8EnumType
  static EnumType ToEnumType(const v8::Local<v8::Value>& val);

  /// Creates enum spec for InitClass. Usefill if enum name is unqualified
#define AT_ADDON_CLASS_ENUM(ENUM) at::node_addon::Util::Enum(#ENUM, ENUM)

  /// Creates enum spec for InitClass. Use this version (instead of macro) if enum name has qualifier.
  static std::pair<std::string, EnumType> Enum(const std::string& name, EnumType val);

// TODO(Art): constants string, integer, floating point

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
   * The class T definition needs to include AT_ADDON_CLASS. (It can be in private section)
   */
  template <class T>
  static bool NewCppInstance(const v8::FunctionCallbackInfo<v8::Value>& args, T* obj);

  /// Helper to make NewCppInstance work (Need this because Wrap() is ObjectWrap's protected member, and NewCppInstance needs to use it.)
#define AT_ADDON_CLASS template <class T> friend bool at::node_addon::Util::NewCppInstance(const v8::FunctionCallbackInfo<v8::Value>&, T*)

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
          const std::string& context, 
          const v8::FunctionCallbackInfo<v8::Value>& args,
          size_t min_num_args, size_t max_num_args,
          CheckFunc&&... checks);
};

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
  PromisePtr<bool> promise_;
  bool queued_ = false;
  std::function<void()> after_fn_;
  void* payload_ = nullptr;

  static void KeepAliveUntilFuture(uv_work_t* work);
  static void Finish(uv_work_t* work, int status);
};

/**
 * A helper class to hold a Node JS callback function object and call it from any thread.
 * This class holds uv_work in the uv event loop, so Node JS program does not exit until
 * the callback is called (or this class is destroyed).
 */
class CallbackInvoker {
 public:
   /// Constructs. This needs to be used in Node JS thread. This ctor does not queue uv_work until callback func is set.
  CallbackInvoker();

   /// Constructs. This needs to be used in Node JS thread. This ctor queues uv_work immediately.
  explicit CallbackInvoker(PersistentFunctionCopyable cb_func);
  explicit CallbackInvoker(v8::Local<v8::Function> cb_func);

  /// If callback function was set in the constructor, or previoud SetCallbackFn call, it'll be replaced.
  void SetCallbackFn(PersistentFunctionCopyable cb_func);
  void SetCallbackFn(v8::Local<v8::Function> cb_func);

  /**
   * Sends a call. It can be called in any thread.
   * This removes uv_work from the event loop, so if no other pending task remains, the program ends.
   * @note This can be called only once. If called twice or more, the later calls are ignored.
   */
  void Call(const std::vector<v8::Local<v8::Value>>& args);

  /// Finishes the uv worker
  void Stop();

  ~CallbackInvoker();

  /// utility function exposed
  static void Invoke(PersistentFunctionCopyable fn, const vector<PersistentValueCopyable>& args);

 private:
  BlockingWorker* worker_ = nullptr;
  PersistentFunctionCopyable cb_func_;
  Mutex args_mutex_;
  bool to_call_ AT_GUARDED_BY(args_mutex_) = false;
  std::vector<PersistentValueCopyable> args_ AT_GUARDED_BY(args_mutex_);

  void Finish();
  void CleanUp();
};


/**
 * A helper class to hold a Node JS callback function objects and call it from any thread.
 * This class holds uv_work in the uv event loop, so Node JS program does not exit until this class is destroyed.
 */
class EventEmitter {
 public:
   /// Constructs. This needs to be used in Node JS thread. This ctor queues uv_work immediately.
  EventEmitter();

  /// Adds listener (there is no way to remove listener, for now)
  void AddListener(PersistentFunctionCopyable cb_func);
  void AddListener(v8::Local<v8::Function> cb_func);

  /**
   * Emits an event to all listeners. It can be called in any thread.
   */
  void Emit(const std::vector<v8::Local<v8::Value>>& args);

  /// Finishes the uv worker
  void Stop();

  ~EventEmitter();

 private:
  BlockingWorker* worker_ = nullptr;
  Mutex mutex_;
  std::vector<PersistentFunctionCopyable> listeners_ AT_GUARDED_BY(mutex_);

  static void EmitEvent(uv_async_t* handle);
};

}  // namespace node_addon
}  // namespace at

#include "addon_util.hpp"

#endif  // AT_ADDON_UTIL_H_
