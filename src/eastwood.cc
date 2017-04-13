#include <vector>
#include <algorithm>
#include <utility>
#include <typeinfo>
#include "eastwood.h"
// cannot include. that creates OpenSSL vs BorringSSL conflicts #include "eastwood/subscribe/subscriber.h"
#include "eastwood/common/time.h"

namespace ew {

using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionCallback;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Uint32;
using v8::Int32;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Value;
using v8::Exception;

using namespace std;
using namespace string_literals;
using namespace chrono_literals;

Persistent<Function> Subscriber::constructor;
Persistent<Function> Subscriber::SubscriberConfig::constructor;

at::Ptr<at::EventLoop> Subscriber::event_loop_;


Subscriber::Subscriber(const FunctionCallbackInfo<Value>& args)
  : config_(SubscriberConfig::NewInstance(args)) {
  if (!event_loop_) {
    event_loop_ = at::EventLoop::New(max<uint32_t>(1, at::EventLoopImpl::GetDefaultNumThreads() - 2));
    // this allocates (# of cores - 2) threads
  }
}

Subscriber::~Subscriber() {
}

void Subscriber::Configure(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  Subscriber* self = ObjectWrap::Unwrap<Subscriber>(args.Holder());
  assert(self);
  args.GetReturnValue().Set(self->config_);
}


namespace {
// helpers

// ThrowException
void ThrowException(Isolate* isolate, Local<Value>(* ex)(Local<String>), const string& msg) {
  assert(isolate);

  isolate->ThrowException(ex(String::NewFromUtf8(isolate, msg.c_str())));
}

// InitClass
pair<string, FunctionCallback> Prototype(const string& name, FunctionCallback func) {
  return make_pair(name, func);
}

pair<string, int> Enum(const string& name, int val) {
  return make_pair(name, val);
}

void SetUpClassProp(Isolate* isolate, Local<FunctionTemplate> tpl) {}

template <class... Rest>
void SetUpClassProp(
          Isolate* isolate, Local<FunctionTemplate> tpl, 
          const pair<string, int>& enm, Rest&&... rest);

template <class... Rest>
void SetUpClassProp(
          Isolate* isolate, Local<FunctionTemplate> tpl, 
          const pair<string, FunctionCallback>& method, Rest&&... rest) {
  NODE_SET_PROTOTYPE_METHOD(tpl, method.first.c_str(), method.second);
  SetUpClassProp(isolate, tpl, forward<Rest>(rest)...);
}

template <class... Rest>
void SetUpClassProp(
          Isolate* isolate, Local<FunctionTemplate> tpl, 
          const pair<string, int>& enm, Rest&&... rest) {
  auto context = isolate->GetCurrentContext();
  Local<Object> obj;
  auto success = obj->DefineOwnProperty(context,
            String::NewFromUtf8(isolate, enm.first.c_str()), Int32::New(isolate, enm.second), v8::ReadOnly);
  if (!success.FromMaybe(false)) {
    ThrowException(isolate, Exception::Error, "Failed to set enum prop " + enm.first);
    return;
  }
  SetUpClassProp(isolate, tpl, forward<Rest>(rest)...);
}

template <class... Prop>  // Prototype(..) or Enum(..)
void InitClass(
          Local<Object> exports, const string& class_name,
          void(* new_func)(const FunctionCallbackInfo<Value>&),
          Persistent<Function>& constructor,
          Prop&&... props) {
  auto isolate = exports->GetIsolate();

  // Prepare constructor template
  auto tpl = FunctionTemplate::New(isolate, new_func);
  tpl->SetClassName(String::NewFromUtf8(isolate, class_name.c_str()));
  tpl->InstanceTemplate()->SetInternalFieldCount(sizeof...(Prop));

  // Prototype
  SetUpClassProp(isolate, tpl, forward<Prop>(props)...);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, class_name.c_str()), tpl->GetFunction());
}

// NewV8Instance
Local<Object> NewV8Instance(Persistent<Function>& constructor, const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  // arg checks are done in ctor
  vector<Local<Value>> isolated_args;
  auto isolated_argc = args.Length();
  isolated_args.reserve(isolated_argc);
  for (auto i = 0 ; i < isolated_argc ; ++i) {
    isolated_args[i] = args[i];
  }
  auto cons = Local<Function>::New(isolate, constructor);
  auto context = isolate->GetCurrentContext();
  return cons->NewInstance(context, isolated_argc, isolated_args.data()).ToLocalChecked();
}

// NewCppInstance
// cre_func needs to Wrap the object
template <class T>
void NewCppInstance(const FunctionCallbackInfo<Value>& args, function<T*()> cre_func) {
  auto isolate = args.GetIsolate();

  if (args.IsConstructCall()) {
    // construct
    auto obj = cre_func();
    args.GetReturnValue().Set(args.This());
  } else {
    ThrowException(isolate, Exception::SyntaxError, "Use 'new' to instantiate "s + typeid(T).name());
    return;
  }
}

// CheckArgs
template <size_t N>
void CheckArg(Isolate* isolate, const string& context, const FunctionCallbackInfo<Value>& args) {}

template <size_t N, class CheckFunc, class... RestOfCheckFunc>
void CheckArg(
        Isolate* isolate, const string& context, const FunctionCallbackInfo<Value>& args,
        CheckFunc&& check, RestOfCheckFunc&&... rest) {

  assert(isolate);
  
  if (!check(args[N])) {
    ThrowException(isolate, Exception::TypeError, context + ": Wrong argument at " + to_string(N));
    return;
  }
  CheckArg<N+1>(isolate, context, args, forward<RestOfCheckFunc>(rest)...);
}

// TODO(Art): do we need context, or stack trace is sufficient?

template <class... CheckFunc> // CheckFunc: bool(const Local<Value>)
void CheckArgs(
  Isolate* isolate, const string& context, const FunctionCallbackInfo<Value>& args, size_t num_args,
  CheckFunc&&... checks) {

  assert(isolate);

  // Check the number of arguments passed.
  if (args.Length() < num_args) {
    // Throw an Error that is passed back to JavaScript
    ThrowException(isolate, Exception::TypeError, 
          context + ": Needs " + to_string(num_args) + " args but given " + to_string(num_args));
    return;
  }

  // Check the argument types
  CheckArg<0>(isolate, context, args, forward<CheckFunc>(checks)...);
}

// ToString
string ToString(const Local<Value> v8str) {
  String::Utf8Value utf8(v8str);
  if (0 < utf8.length()) return string(*utf8, utf8.length());
  return "";
}

// ToInt32
int32_t ToInt32(Isolate* isolate, Local<Value> val) {
  assert(isolate);
  auto maybeValue = val->ToInt32(isolate->GetCurrentContext());
  Local<Int32> value;
  if (maybeValue.ToLocal(&value)) {
    return value->Value();
  }
  throw logic_error("Value is not int32");
}

// ToUint32
uint32_t ToUint32(Isolate* isolate, Local<Value> val) {
  assert(isolate);
  auto maybeValue = val->ToUint32(isolate->GetCurrentContext());
  Local<Uint32> value;
  if (maybeValue.ToLocal(&value)) {
    return value->Value();
  }
  throw logic_error("Value is not uint32");
}

// ToDouble
double ToDouble(Isolate* isolate, Local<Value> val) {
  assert(isolate);
  auto maybeValue = val->ToNumber(isolate->GetCurrentContext());
  Local<Number> value;
  if (maybeValue.ToLocal(&value)) {
    return value->Value();
  }
  throw logic_error("Value is not double");
}

// ToBool
bool ToBool(Isolate* isolate, Local<Value> val) {
  assert(isolate);
  return val->IsTrue();
}
}  // anonymous namespace

void Subscriber::SubscriberConfig::Bixby(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  CheckArgs(isolate, "bixby", args, 2,
    [](const Local<Value> arg0) { return arg0->IsString(); },
    [](const Local<Value> arg1) { return arg1->IsUint32(); });

// TODO(Art): No need ObjectWrap qualifier, it's the base class
  SubscriberConfig* self = ObjectWrap::Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  uint32_t port = 0;
  try {
    port = ToUint32(isolate, args[1]);
  } catch (const exception& ex) {
// TOODO(Art): do we need function name here or stack trace is sufficient?
    ThrowException(isolate, Exception::TypeError, "bixby port error. "s + ex.what());
    return;
  }
  self->bixby_endpoint_ = at::Endpoint(ToString(args[0]), port);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::BixbyAllocator(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  CheckArgs(isolate, "bixbyAllocator", args, 3,
    [](const Local<Value> arg0) { return arg0->IsString(); },
    [](const Local<Value> arg1) { return arg1->IsUint32(); },
    [](const Local<Value> arg2) { return arg2->IsString(); });

  SubscriberConfig* self = ObjectWrap::Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  uint32_t port = 0;
  try {
    port = ToUint32(isolate, args[1]);
  } catch (const exception& ex) {
    ThrowException(isolate, Exception::TypeError, "bixbyAllocator port error. "s + ex.what());
    return;
  }
  self->allocator_endpoint_ = at::Endpoint(ToString(args[0]), port);
  self->alloc_location_ = ToString(args[2]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::StreamNotifier(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  CheckArgs(isolate, "streamNotifier", args, 5,
    [](const Local<Value> arg0) { return arg0->IsString(); },
    [](const Local<Value> arg1) { return arg1->IsUint32(); },
    [](const Local<Value> arg2) { return arg2->IsString(); },
    [](const Local<Value> arg3) { return arg3->IsBoolean(); },  
    [](const Local<Value> arg4) { return arg4->IsBoolean(); });

  SubscriberConfig* self = ObjectWrap::Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  uint32_t port = 0;
  try {
    port = ToUint32(isolate, args[1]);
  } catch (const exception& ex) {
    ThrowException(isolate, Exception::TypeError, "streamNotifier port error. "s + ex.what());
    return;
  }
  self->notifier_endpoint_ = at::Endpoint(ToString(args[0]), port);
  self->tag_ = ToString(args[2]);
  bool use_tls = true;
  try {
    use_tls = ToBool(isolate, args[3]);
  } catch (const exception& ex) {
    ThrowException(isolate, Exception::TypeError, "streamNotifier useTls error. "s + ex.what());
    return;
  }
  bool cert_check = true;
  try {
    cert_check = ToBool(isolate, args[4]);
  } catch (const exception& ex) {
    ThrowException(isolate, Exception::TypeError, "streamNotifier certCheck error. "s + ex.what());
    return;
  }
  self->notifier_use_tls_ = use_tls;
  self->notifier_cert_check_ = cert_check;
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::Duration(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  CheckArgs(isolate, "duration", args, 1,
    [](const Local<Value> arg0) { return arg0->IsString(); });

  SubscriberConfig* self = ObjectWrap::Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->duration_ = at::eastwood::DurationFromString(ToString(args[0]));
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::UserId(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  CheckArgs(isolate, "userId", args, 1,
    [](const Local<Value> arg0) { return arg0->IsString(); });

  SubscriberConfig* self = ObjectWrap::Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->user_id_ = ToString(args[0]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::StreamUrl(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  CheckArgs(isolate, "streamUrl", args, 1,
    [](const Local<Value> arg0) { return arg0->IsString(); });

  SubscriberConfig* self = ObjectWrap::Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->stream_url_ = ToString(args[0]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::CertCheck(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  CheckArgs(isolate, "certCheck", args, 1,
    [](const Local<Value> arg0) { return arg0->IsBoolean(); });

  SubscriberConfig* self = ObjectWrap::Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->cert_check_ = args[0]->ToBoolean()->Value();
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::AuthSecret(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  CheckArgs(isolate, "authSecret", args, 1,
    [](const Local<Value> arg0) { return arg0->IsString(); });

  SubscriberConfig* self = ObjectWrap::Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->auth_secret_ = ToString(args[0]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::PrintFrameInfo(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  CheckArgs(isolate, "printFrameInfo", args, 1,
    [](const Local<Value> arg0) { return arg0->IsBoolean(); });

  SubscriberConfig* self = ObjectWrap::Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->print_frame_info_ = args[0]->ToBoolean()->Value();
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::AudioSink(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  CheckArgs(isolate, "audioSink", args, 1,
    [](const Local<Value> arg0) { return arg0->IsUint32(); });

  SubscriberConfig* self = ObjectWrap::Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  int32_t sink = 0;
  try {
    sink = ToInt32(isolate, args[0]);
  } catch (const exception& ex) {
    ThrowException(isolate, Exception::TypeError, "audioSink type error. "s + ex.what());
    return;
  }
  if (2 <= args.Length()) {
    if (!args[1]->IsString()) {
      ThrowException(isolate, Exception::SyntaxError, "Audio sink param needs to be string");
      return;
    }
    self->audio_sink_param_ = ToString(args[1]);
  }
  self->audio_sink_ = static_cast<AudioSinkType>(sink);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::VideoSink(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  CheckArgs(isolate, "videoSink", args, 1,
    [](const Local<Value> arg0) { return arg0->IsUint32(); });

  SubscriberConfig* self = ObjectWrap::Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  int32_t sink = 0;
  try {
    sink = ToInt32(isolate, args[0]);
  } catch (const exception& ex) {
    ThrowException(isolate, Exception::TypeError, "videoSink type error. "s + ex.what());
    return;
  }
  if (2 <= args.Length()) {
    if (!args[1]->IsString()) {
      ThrowException(isolate, Exception::SyntaxError, "Video sink param needs to be string");
      return;
    }
    self->video_sink_param_ = ToString(args[1]);
  }
  self->video_sink_ = static_cast<VideoSinkType>(sink);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::SubscriptionErrorRetry(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  CheckArgs(isolate, "subscriptionErrorRetry", args, 3,
    [](const Local<Value> arg0) { return arg0->IsUint32(); },
    [](const Local<Value> arg1) { return arg1->IsUint32(); },
    [](const Local<Value> arg2) { return arg2->IsNumber(); });

  SubscriberConfig* self = ObjectWrap::Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  uint32_t max_retries = 0;
  try {
    max_retries = ToUint32(isolate, args[0]);
  } catch (const exception& ex) {
    ThrowException(isolate, Exception::TypeError, "subscriptionErrorRetry maxRetries error. "s + ex.what());
    return;
  }
  self->err_max_retries_ = max_retries;
  uint32_t delay_ms = 0;
  try {
    delay_ms = ToUint32(isolate, args[1]);
  } catch (const exception& ex) {
    ThrowException(isolate, Exception::TypeError, "subscriptionErrorRetry delayMs error. "s + ex.what());
    return;
  }
  self->err_retry_init_delay_ms_ = delay_ms;
  double progression = 0;
  try {
    progression = ToDouble(isolate, args[2]);
  } catch (const exception& ex) {
    ThrowException(isolate, Exception::TypeError, "subscriptionErrorRetry delayProgression error. "s + ex.what());
    return;
  }
  self->err_retry_delay_progression_ = progression;
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::On(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  CheckArgs(isolate, "on", args, 2,
    [](const Local<Value> arg0) {
      if (!arg0->IsString()) return false;
      auto event = ToString(arg0);
      return ("started" == event
           || "ended" == event
           || "error" == event);
    },
    [](const Local<Value> arg1) { return arg1->IsFunction(); });

  Subscriber* self = ObjectWrap::Unwrap<Subscriber>(args.Holder());
  assert(self);

  auto event = ToString(args[0]);
  if ("started" == event) {
    self->started_event_listeners_.emplace_back(Local<Function>::Cast(args[1]));
  } else if ("ended" == event) {
    self->ended_event_listeners_.emplace_back(Local<Function>::Cast(args[1]));
  } else if ("error" == event) {
    self->error_event_listeners_.emplace_back(Local<Function>::Cast(args[1]));
  } else {
    // can't happen because the check above passed
    assert(false);
  }
}

void Subscriber::Start(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  Subscriber* self = ObjectWrap::Unwrap<Subscriber>(args.Holder());
  assert(self);
  SubscriberConfig* config = ObjectWrap::Unwrap<SubscriberConfig>(self->config_);
  assert(config);
  config->VerifyConfig(isolate);

  // TODO(Art): start!
}

void Subscriber::Stop(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  Subscriber* self = ObjectWrap::Unwrap<Subscriber>(args.Holder());
  assert(self);

  // TODO(Art): stop!
}

void Subscriber::SubscriberConfig::VerifyConfig(Isolate* isolate) const {
  VerifyBixbyConfig(isolate);
  VerifyAllocatorConfig(isolate);
  VerifyNotifierConfig(isolate);
  VerifyDurationConfig(isolate);
  VerifyUserIdConfig(isolate);
  VerifyStreamUrlConfig(isolate);
  VerifyTagConfig(isolate);
  VerifyAudioSinkConfig(isolate);
  VerifyVideoSinkConfig(isolate);
  VerifyErrorRetryConfig(isolate);
  VerifyConfigCombinations(isolate);
}

void Subscriber::SubscriberConfig::VerifyBixbyConfig(Isolate* isolate) const {
  assert(isolate);
  // no specific check
}

void Subscriber::SubscriberConfig::VerifyAllocatorConfig(Isolate* isolate) const {
  assert(isolate);
  if (at::Endpoint() != allocator_endpoint_) {
    if (alloc_location_.empty()) {
      ThrowException(isolate, Exception::Error, "Allocation location is not set");
      return;
    }
  }
}

void Subscriber::SubscriberConfig::VerifyNotifierConfig(Isolate* isolate) const {
  assert(isolate);
  if (at::Endpoint() != notifier_endpoint_ && tag_.empty()) {
    ThrowException(isolate, Exception::RangeError, "Stream notifier needs Tag");
    return;
  }
}

void Subscriber::SubscriberConfig::VerifyDurationConfig(Isolate* isolate) const {
  assert(isolate);
  if (0s == duration_) {
    ThrowException(isolate, Exception::RangeError, "Duration is not set");
    return;
  }
}

void Subscriber::SubscriberConfig::VerifyUserIdConfig(Isolate* isolate) const {
  assert(isolate);
  // no specific check
}

void Subscriber::SubscriberConfig::VerifyStreamUrlConfig(Isolate* isolate) const {
  assert(isolate);
  // no specific check
}

void Subscriber::SubscriberConfig::VerifyTagConfig(Isolate* isolate) const {
  assert(isolate);
  // no specific check
}

void Subscriber::SubscriberConfig::VerifyAudioSinkConfig(Isolate* isolate) const {
  assert(isolate);
  switch (audio_sink_) {
    case AudioSinkType::AudioSinkNone:
      break;
    case AudioSinkType::AudioSinkFile:
      if (audio_sink_param_.empty()) {
        ThrowException(isolate, Exception::SyntaxError, "File audio sink needs filename");
        return;
      }
      break;
    case AudioSinkType::AudioSinkFFMpeg:
      break;
    default:
      ThrowException(isolate, Exception::RangeError, "Invalid audio sink type");
      break;
  }
}

void Subscriber::SubscriberConfig::VerifyVideoSinkConfig(Isolate* isolate) const {
  assert(isolate);
  switch (video_sink_) {
    case VideoSinkType::VideoSinkNone:
      break;
    case VideoSinkType::VideoSinkFile:
      if (video_sink_param_.empty()) {
        ThrowException(isolate, Exception::SyntaxError, "File video sink needs filename");
        return;
      }
      break;
    case VideoSinkType::VideoSinkFFMpeg:
      break;
    default:
      ThrowException(isolate, Exception::RangeError, "Invalid video sink type");
      break;
  }
}

void Subscriber::SubscriberConfig::VerifyErrorRetryConfig(Isolate* isolate) const {
  assert(isolate);
  if (0 == err_max_retries_) return;  // no error retry
  if (err_retry_delay_progression_ < 1.0) {
    ThrowException(isolate, Exception::RangeError, "Subscription error retry progression factor needs to be 1.0 or more");
  }
}

void Subscriber::SubscriberConfig::VerifyConfigCombinations(Isolate* isolate) const {
  assert(isolate);
  if (at::Endpoint() != bixby_endpoint_ && at::Endpoint() != allocator_endpoint_) {
    ThrowException(isolate, Exception::Error, "Bixby endpoint and Allocator endpoint are mutually exclusive");
    return;
  }
  if (at::Endpoint() != notifier_endpoint_ && !stream_url_.empty()) {
    ThrowException(isolate, Exception::Error, "Stream notifier endpoint and Stream URL are mutually exclusive");
    return;
  }
  if (!tag_.empty() && !stream_url_.empty()) {
    ThrowException(isolate, Exception::Error, "Tag and Stream URL are mutually exclusive");
    return;
  }
}

string Subscriber::SubscriberConfig::AudioSinkString(AudioSinkType sink) {
  switch (sink) {
    case AudioSinkType::AudioSinkNone:
      return "none";
    case AudioSinkType::AudioSinkFile:
      return "file";
    case AudioSinkType::AudioSinkFFMpeg:
      return "ffmpeg";
    default:
      return "invalid" + to_string(static_cast<int>(sink));
  }
}

string Subscriber::SubscriberConfig::VideoSinkString(VideoSinkType sink) {
  switch (sink) {
    case VideoSinkType::VideoSinkNone:
      return "none";
    case VideoSinkType::VideoSinkFile:
      return "file";
    case VideoSinkType::VideoSinkFFMpeg:
      return "ffmpeg";
    default:
      return "invalid" + to_string(static_cast<int>(sink));
  }
}

ostream& operator<< (ostream& ost, const Subscriber::SubscriberConfig& config) {
  return ost
    << "Bixby:{" << config.bixby_endpoint_ << "} "
    << "Allcator:{" << config.allocator_endpoint_ << " loc:" << config.alloc_location_ << "} "
    << "Notifier:{" << config.notifier_endpoint_ << " tag: " << config.tag_
      << " TLS:" << (config.notifier_use_tls_ ? "yes" : "no")
      << " Cert:" << (config.notifier_cert_check_ ? "yes" : "no") << "} "
    << "Duration:" << chrono::duration_cast<chrono::milliseconds>(config.duration_).count() << "ms "
    << "UserId:" << config.user_id_ << " "
    << "StreamURL:" << config.stream_url_ << " "
    << "Cert:" << (config.cert_check_ ? "yes" : "no") << " "
    << "Secret:" << config.auth_secret_ << " "
    << "FrameInfo:" << (config.print_frame_info_ ? "yes" : "no") << " "
    << "Audio:{ Sink:" << Subscriber::SubscriberConfig::AudioSinkString(config.audio_sink_) << " "
      << "Param:" << config.audio_sink_param_ << "} "
    << "Video:{ Sink:" << Subscriber::SubscriberConfig::VideoSinkString(config.video_sink_) << " "
      << "Param:" << config.video_sink_param_ << "} "
    << "Retry:{ Max:" << config.err_max_retries_ 
      << " Init_delay:" << config.err_retry_init_delay_ms_ << "ms "
      << " Progression:" << config.err_retry_delay_progression_ << "}";
}

void Subscriber::DumpConfig(const v8::FunctionCallbackInfo<v8::Value>& args) {
  auto isolate = args.GetIsolate();
  Subscriber* self = ObjectWrap::Unwrap<Subscriber>(args.Holder());
  assert(self);
  SubscriberConfig* config = ObjectWrap::Unwrap<SubscriberConfig>(self->config_);
  assert(config);
  ostringstream out;
  out << *config;
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, out.str().c_str()));
}


Subscriber::SubscriberConfig::SubscriberConfig() {
}

void Subscriber::SubscriberConfig::Init(Local<Object> exports) {
  // auto isolate = exports->GetIsolate();

  // // Prepare constructor template
  // auto tpl = FunctionTemplate::New(isolate, New);
  // tpl->SetClassName(String::NewFromUtf8(isolate, "SubscriberConfig"));
  // tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // // Prototype
  // NODE_SET_PROTOTYPE_METHOD(tpl, "add", Add);
  // NODE_SET_PROTOTYPE_METHOD(tpl, "getValue", GetValue);

  // constructor.Reset(isolate, tpl->GetFunction());
  // exports->Set(String::NewFromUtf8(isolate, "Subscriber"),
  //              tpl->GetFunction());
  InitClass(exports, "SubscriberConfig", New, constructor,
            Prototype("bixby"s, Bixby),
            Prototype("bixbyAllocator"s, BixbyAllocator),
            Prototype("streamNotifier"s, StreamNotifier),
            Prototype("duration"s, Duration),
            Prototype("userId"s, UserId),
            Prototype("streamUrl"s, StreamUrl),
            Prototype("certCheck"s, CertCheck),
            Prototype("authSecret"s, AuthSecret),
            Prototype("printFrameInfo"s, PrintFrameInfo),
            Prototype("audioSink"s, AudioSink),
            Prototype("videoSink"s, VideoSink),
            Prototype("subscriptionErrorRetry"s, SubscriptionErrorRetry));
}

Local<Object> Subscriber::SubscriberConfig::NewInstance(const FunctionCallbackInfo<Value>& args) {
  // auto isolate = args.GetIsolate();

  // // arg checks are done in ctor
  // vector<Local<Value>> isolated_args;
  // auto isolated_argc = args.Length();
  // isolated_args.reserve(isolated_argc);
  // for (auto i = 0 ; i < isolated_argc ; ++i) {
  //   isolated_args[i] = args[i];
  // }
  // auto cons = Local<Function>::New(isolate, constructor);
  // auto context = isolate->GetCurrentContext();
  // return cons->NewInstance(context, isolated_argc, argv).ToLocalChecked();
  return NewV8Instance(constructor, args);
}

void Subscriber::SubscriberConfig::New(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  // if (args.IsConstructCall()) {
  //   // Check the number of arguments passed.
  //   if (0 < args.Length()) {
  //     // Throw an Error that is passed back to JavaScript
  //     throwException(isolate, Exception::TypeError, "SubscriberConfig constructor does not take argument");
  //     return;
  //   }

  //   // construct
  //   auto obj = new SubscriberConfig(args);
  //   obj->Wrap(args.This());
  //   args.GetReturnValue().Set(args.This());
  // } else {
  //   throwException(isolate, Exception::SyntaxError, "Use 'new' to instantiate EastWood");
  //   return;
  // }
  NewCppInstance<SubscriberConfig>(args, [&args]() {
    auto obj = new SubscriberConfig();
    obj->Wrap(args.This());
    return obj;
  });
}



void Subscriber::Init(Local<Object> exports) {
  auto isolate = exports->GetIsolate();

  // // Prepare constructor template
  // auto tpl = FunctionTemplate::New(isolate, New);
  // tpl->SetClassName(String::NewFromUtf8(isolate, "Subscriber"));
  // tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // // Prototype
  // NODE_SET_PROTOTYPE_METHOD(tpl, "add", Add);
  // NODE_SET_PROTOTYPE_METHOD(tpl, "getValue", GetValue);

  // constructor.Reset(isolate, tpl->GetFunction());
  // exports->Set(String::NewFromUtf8(isolate, "Subscriber"),
  //              tpl->GetFunction());

  InitClass(exports, "Subscriber", New, constructor,
            Prototype("configure"s, Configure),
            Prototype("on"s, On),
            Prototype("start"s, Start),
            Prototype("stop"s, Stop),
            Prototype("dumpConfig"s, DumpConfig),
            Enum("AudioSinkNone"s, SubscriberConfig::AudioSinkType::AudioSinkNone),
            Enum("AudioSinkFile"s, SubscriberConfig::AudioSinkType::AudioSinkFile),
            Enum("AudioSinkFFMpeg"s, SubscriberConfig::AudioSinkType::AudioSinkFFMpeg),
            Enum("VideoSinkNone"s, SubscriberConfig::VideoSinkType::VideoSinkNone),
            Enum("VideoSinkFile"s, SubscriberConfig::VideoSinkType::VideoSinkFile),
            Enum("VideoSinkFFMpeg"s, SubscriberConfig::VideoSinkType::VideoSinkFFMpeg)
  );

  SubscriberConfig::Init(exports);
}

Local<Object> Subscriber::NewInstance(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  // pass copy of args
  vector<Local<Value>> isolated_args;
  auto isolated_argc = args.Length();
  isolated_args.reserve(isolated_argc);
  for (auto i = 0 ; i < isolated_argc ; ++i) {
    isolated_args[i] = args[i];
  }
  auto cons = Local<Function>::New(isolate, constructor);
  auto context = isolate->GetCurrentContext();
  return cons->NewInstance(context, isolated_argc, isolated_args.data()).ToLocalChecked();
}

void Subscriber::New(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  if (args.IsConstructCall()) {
    // Check the number of arguments passed.
    if (0 < args.Length()) {
      // Throw an Error that is passed back to JavaScript
      ThrowException(isolate, Exception::TypeError, "Subscriber constructor does not take argument");
      return;
    }

    // construct
    auto obj = new Subscriber(args);
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    ThrowException(isolate, Exception::SyntaxError, "Use 'new' to instantiate EastWood");
    return;
  }
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

  Subscriber::Init(exports);
}

void EastWood::New(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new EastWood(...)`
    auto obj = new EastWood();
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    ThrowException(isolate, Exception::SyntaxError, "Use 'new' to instantiate EastWood");
    return;
  }
}

// Subscriber(double)
void EastWood::CreateSubscriber(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  // passing all args
  args.GetReturnValue().Set(Subscriber::NewInstance(args));
}

}  // namespace ew
