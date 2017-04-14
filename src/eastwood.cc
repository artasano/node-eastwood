#include <vector>
#include <algorithm>
#include <utility>
#include <typeinfo>
#include "eastwood.h"
// cannot include. that creates OpenSSL vs BorringSSL conflicts #include "eastwood/subscribe/subscriber.h"
#include "eastwood/common/time.h"
#include "addon_util.h"

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
using v8::Boolean;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Symbol;
using v8::Value;
using v8::Exception;
using v8::PropertyAttribute;

using namespace std;
using namespace string_literals;
using namespace chrono_literals;

using at::node_addon::Util;


Persistent<Function> Subscriber::constructor;
Persistent<Function> Subscriber::SubscriberConfig::constructor;

at::Ptr<at::EventLoop> EastWood::event_loop_;


// --------------------------------------------

Persistent<Function> EastWood::constructor;

EastWood::EastWood() {
  if (!event_loop_) {
    event_loop_ = at::EventLoop::New(max<uint32_t>(1, at::EventLoopImpl::GetDefaultNumThreads() - 2));
    // this allocates (# of cores - 2) threads
  }
}

EastWood::~EastWood() {
}

string EastWood::AudioSinkString(AudioSinkType sink) {
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

string EastWood::VideoSinkString(VideoSinkType sink) {
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

void EastWood::Init(Local<Object> exports) {
  Util::InitClass(exports, "EastWood", New, constructor,
          Util::Prototype("createSubscriber"s, CreateSubscriber),
          AT_ADDON_CLASS_ENUM(AudioSinkNone),
          AT_ADDON_CLASS_ENUM(AudioSinkFile),
          AT_ADDON_CLASS_ENUM(AudioSinkFFMpeg),
          AT_ADDON_CLASS_ENUM(VideoSinkNone),
          AT_ADDON_CLASS_ENUM(VideoSinkFile),
          AT_ADDON_CLASS_ENUM(VideoSinkFFMpeg)
  );

  Subscriber::Init(exports);
}

void EastWood::New(const FunctionCallbackInfo<Value>& args) {
  AT_ADDON_NEW_CPP_INSTANCE(args, EastWood, );
}

void EastWood::CreateSubscriber(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  args.GetReturnValue().Set(Subscriber::NewInstance(args));
}

// --------------------------------------------

Subscriber::Subscriber(const FunctionCallbackInfo<Value>& args)
  : config_(args.GetIsolate(), SubscriberConfig::NewInstance(args)) {
}

Subscriber::~Subscriber() {
  config_.Reset();
}

void Subscriber::Configure(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  Subscriber* self = Unwrap<Subscriber>(args.Holder());
  assert(self);
  args.GetReturnValue().Set(self->config_.Get(isolate));
}


void Subscriber::SubscriberConfig::Bixby(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  Util::CheckArgs(isolate, "bixby", args, 2,
    [](const Local<Value> arg0) { return arg0->IsString(); },
    [](const Local<Value> arg1) { return arg1->IsUint32(); });

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  uint32_t port = 0;
  try {
    port = Util::ToUint32(isolate, args[1]);
  } catch (const exception& ex) {
    Util::ThrowException(args, Exception::TypeError, "bixby port error. "s + ex.what());
    return;
  }
  self->bixby_endpoint_ = at::Endpoint(Util::ToString(args[0]), port);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::BixbyAllocator(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  Util::CheckArgs(isolate, "bixbyAllocator", args, 3,
    [](const Local<Value> arg0) { return arg0->IsString(); },
    [](const Local<Value> arg1) { return arg1->IsUint32(); },
    [](const Local<Value> arg2) { return arg2->IsString(); });

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  uint32_t port = 0;
  try {
    port = Util::ToUint32(isolate, args[1]);
  } catch (const exception& ex) {
    Util::ThrowException(args, Exception::TypeError, "bixbyAllocator port error. "s + ex.what());
    return;
  }
  self->allocator_endpoint_ = at::Endpoint(Util::ToString(args[0]), port);
  self->alloc_location_ = Util::ToString(args[2]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::StreamNotifier(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  Util::CheckArgs(isolate, "streamNotifier", args, 5,
    [](const Local<Value> arg0) { return arg0->IsString(); },
    [](const Local<Value> arg1) { return arg1->IsUint32(); },
    [](const Local<Value> arg2) { return arg2->IsString(); },
    [](const Local<Value> arg3) { return arg3->IsBoolean(); },  
    [](const Local<Value> arg4) { return arg4->IsBoolean(); });

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  uint32_t port = 0;
  try {
    port = Util::ToUint32(isolate, args[1]);
  } catch (const exception& ex) {
    Util::ThrowException(args, Exception::TypeError, "streamNotifier port error. "s + ex.what());
    return;
  }
  self->notifier_endpoint_ = at::Endpoint(Util::ToString(args[0]), port);
  self->tag_ = Util::ToString(args[2]);
  bool use_tls = true;
  try {
    use_tls = Util::ToBool(isolate, args[3]);
  } catch (const exception& ex) {
    Util::ThrowException(args, Exception::TypeError, "streamNotifier useTls error. "s + ex.what());
    return;
  }
  bool cert_check = true;
  try {
    cert_check = Util::ToBool(isolate, args[4]);
  } catch (const exception& ex) {
    Util::ThrowException(args, Exception::TypeError, "streamNotifier certCheck error. "s + ex.what());
    return;
  }
  self->notifier_use_tls_ = use_tls;
  self->notifier_cert_check_ = cert_check;
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::Duration(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  Util::CheckArgs(isolate, "duration", args, 1,
    [](const Local<Value> arg0) { return arg0->IsString(); });

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->duration_ = at::eastwood::DurationFromString(Util::ToString(args[0]));
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::UserId(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  Util::CheckArgs(isolate, "userId", args, 1,
    [](const Local<Value> arg0) { return arg0->IsString(); });

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->user_id_ = Util::ToString(args[0]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::StreamUrl(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  Util::CheckArgs(isolate, "streamUrl", args, 1,
    [](const Local<Value> arg0) { return arg0->IsString(); });

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->stream_url_ = Util::ToString(args[0]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::CertCheck(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  Util::CheckArgs(isolate, "certCheck", args, 1,
    [](const Local<Value> arg0) { return arg0->IsBoolean(); });

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->cert_check_ = Util::ToBool(isolate, args[0]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::AuthSecret(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  Util::CheckArgs(isolate, "authSecret", args, 1,
    [](const Local<Value> arg0) { return arg0->IsString(); });

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->auth_secret_ = Util::ToString(args[0]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::PrintFrameInfo(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  Util::CheckArgs(isolate, "printFrameInfo", args, 1,
    [](const Local<Value> arg0) { return arg0->IsBoolean(); });

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->print_frame_info_ = Util::ToBool(isolate, args[0]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::AudioSink(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  Util::CheckArgs(isolate, "audioSink", args, 1,
    [](const Local<Value> arg0) { return arg0->IsUint32(); });

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  int32_t sink = 0;
  try {
    sink = Util::ToInt32(isolate, args[0]);
  } catch (const exception& ex) {
    Util::ThrowException(args, Exception::TypeError, "audioSink type error. "s + ex.what());
    return;
  }
  if (2 <= args.Length()) {
    if (!args[1]->IsString()) {
      Util::ThrowException(args, Exception::SyntaxError, "Audio sink param needs to be string");
      return;
    }
    self->audio_sink_param_ = Util::ToString(args[1]);
  }
  self->audio_sink_ = static_cast<EastWood::AudioSinkType>(sink);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::VideoSink(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  Util::CheckArgs(isolate, "videoSink", args, 1,
    [](const Local<Value> arg0) { return arg0->IsUint32(); });

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  int32_t sink = 0;
  try {
    sink = Util::ToInt32(isolate, args[0]);
  } catch (const exception& ex) {
    Util::ThrowException(args, Exception::TypeError, "videoSink type error. "s + ex.what());
    return;
  }
  if (2 <= args.Length()) {
    if (!args[1]->IsString()) {
      Util::ThrowException(args, Exception::SyntaxError, "Video sink param needs to be string");
      return;
    }
    self->video_sink_param_ = Util::ToString(args[1]);
  }
  self->video_sink_ = static_cast<EastWood::VideoSinkType>(sink);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::SubscriptionErrorRetry(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  Util::CheckArgs(isolate, "subscriptionErrorRetry", args, 3,
    [](const Local<Value> arg0) { return arg0->IsUint32(); },
    [](const Local<Value> arg1) { return arg1->IsUint32(); },
    [](const Local<Value> arg2) { return arg2->IsNumber(); });

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  uint32_t max_retries = 0;
  try {
    max_retries = Util::ToUint32(isolate, args[0]);
  } catch (const exception& ex) {
    Util::ThrowException(args, Exception::TypeError,
                "subscriptionErrorRetry maxRetries error. "s + ex.what());
    return;
  }
  self->err_max_retries_ = max_retries;
  uint32_t delay_ms = 0;
  try {
    delay_ms = Util::ToUint32(isolate, args[1]);
  } catch (const exception& ex) {
    Util::ThrowException(args, Exception::TypeError,
                "subscriptionErrorRetry delayMs error. "s + ex.what());
    return;
  }
  self->err_retry_init_delay_ms_ = delay_ms;
  double progression = 0;
  try {
    progression = Util::ToDouble(isolate, args[2]);
  } catch (const exception& ex) {
    Util::ThrowException(args, Exception::TypeError,
                "subscriptionErrorRetry delayProgression error. "s + ex.what());
    return;
  }
  self->err_retry_delay_progression_ = progression;
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::On(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  Util::CheckArgs(isolate, "on", args, 2,
    [](const Local<Value> arg0) {
      if (!arg0->IsString()) return false;
      auto event = Util::ToString(arg0);
      return ("started" == event
           || "ended" == event
           || "error" == event);
    },
    [](const Local<Value> arg1) { return arg1->IsFunction(); });

  Subscriber* self = Unwrap<Subscriber>(args.Holder());
  assert(self);

  auto event = Util::ToString(args[0]);
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

  Subscriber* self = Unwrap<Subscriber>(args.Holder());
  assert(self);
  SubscriberConfig* config = Unwrap<SubscriberConfig>(self->config_.Get(isolate));
  assert(config);
  config->VerifyConfig(args);

  // TODO(Art): start!
}

void Subscriber::Stop(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  Subscriber* self = Unwrap<Subscriber>(args.Holder());
  assert(self);

  // TODO(Art): stop!
}

void Subscriber::SubscriberConfig::VerifyConfig(const FunctionCallbackInfo<Value>& args) const {
  VerifyBixbyConfig(args);
  VerifyAllocatorConfig(args);
  VerifyNotifierConfig(args);
  VerifyDurationConfig(args);
  VerifyUserIdConfig(args);
  VerifyStreamUrlConfig(args);
  VerifyTagConfig(args);
  VerifyAudioSinkConfig(args);
  VerifyVideoSinkConfig(args);
  VerifyErrorRetryConfig(args);
  VerifyConfigCombinations(args);
}

void Subscriber::SubscriberConfig::VerifyBixbyConfig(const FunctionCallbackInfo<Value>& args) const {
  // no specific check
}

void Subscriber::SubscriberConfig::VerifyAllocatorConfig(const FunctionCallbackInfo<Value>& args) const {
  if (at::Endpoint() != allocator_endpoint_) {
    if (alloc_location_.empty()) {
      Util::ThrowException(args, Exception::Error, "Allocation location is not set");
      return;
    }
  }
}

void Subscriber::SubscriberConfig::VerifyNotifierConfig(const FunctionCallbackInfo<Value>& args) const {
  if (at::Endpoint() != notifier_endpoint_ && tag_.empty()) {
    Util::ThrowException(args, Exception::RangeError, "Stream notifier needs Tag");
    return;
  }
}

void Subscriber::SubscriberConfig::VerifyDurationConfig(const FunctionCallbackInfo<Value>& args) const {
  if (0s == duration_) {
    Util::ThrowException(args, Exception::RangeError, "Duration is not set");
    return;
  }
}

void Subscriber::SubscriberConfig::VerifyUserIdConfig(const FunctionCallbackInfo<Value>& args) const {
  // no specific check
}

void Subscriber::SubscriberConfig::VerifyStreamUrlConfig(const FunctionCallbackInfo<Value>& args) const {
  // no specific check
}

void Subscriber::SubscriberConfig::VerifyTagConfig(const FunctionCallbackInfo<Value>& args) const {
  // no specific check
}

void Subscriber::SubscriberConfig::VerifyAudioSinkConfig(const FunctionCallbackInfo<Value>& args) const {
  switch (audio_sink_) {
    case EastWood::AudioSinkType::AudioSinkNone:
      break;
    case EastWood::AudioSinkType::AudioSinkFile:
      if (audio_sink_param_.empty()) {
        Util::ThrowException(args, Exception::SyntaxError, "File audio sink needs filename");
        return;
      }
      break;
    case EastWood::AudioSinkType::AudioSinkFFMpeg:
      break;
    default:
      Util::ThrowException(args, Exception::RangeError, "Invalid audio sink type");
      break;
  }
}

void Subscriber::SubscriberConfig::VerifyVideoSinkConfig(const FunctionCallbackInfo<Value>& args) const {
  switch (video_sink_) {
    case EastWood::VideoSinkType::VideoSinkNone:
      break;
    case EastWood::VideoSinkType::VideoSinkFile:
      if (video_sink_param_.empty()) {
        Util::ThrowException(args, Exception::SyntaxError, "File video sink needs filename");
        return;
      }
      break;
    case EastWood::VideoSinkType::VideoSinkFFMpeg:
      break;
    default:
      Util::ThrowException(args, Exception::RangeError, "Invalid video sink type");
      break;
  }
}

void Subscriber::SubscriberConfig::VerifyErrorRetryConfig(const FunctionCallbackInfo<Value>& args) const {
  if (0 == err_max_retries_) return;  // no error retry
  if (err_retry_delay_progression_ < 1.0) {
    Util::ThrowException(args, Exception::RangeError, "Subscription error retry progression factor needs to be 1.0 or more");
  }
}

void Subscriber::SubscriberConfig::VerifyConfigCombinations(const FunctionCallbackInfo<Value>& args) const {
  if (at::Endpoint() != bixby_endpoint_ && at::Endpoint() != allocator_endpoint_) {
    Util::ThrowException(args, Exception::Error,
                "Bixby endpoint and Allocator endpoint are mutually exclusive");
    return;
  }
  if (at::Endpoint() != notifier_endpoint_ && !stream_url_.empty()) {
    Util::ThrowException(args, Exception::Error,
                "Stream notifier endpoint and Stream URL are mutually exclusive");
    return;
  }
  if (!tag_.empty() && !stream_url_.empty()) {
    Util::ThrowException(args, Exception::Error, "Tag and Stream URL are mutually exclusive");
    return;
  }
  if ((audio_sink_ == EastWood::AudioSinkType::AudioSinkFFMpeg && video_sink_
        != EastWood::VideoSinkType::VideoSinkFFMpeg)
   || (audio_sink_ != EastWood::AudioSinkType::AudioSinkFFMpeg && video_sink_
        == EastWood::VideoSinkType::VideoSinkFFMpeg)) {
    Util::ThrowException(args, Exception::Error, "Audio sink and Video sink type mismatch");
    return;
  }
}

void Subscriber::SubscriberConfig::ToObject(const v8::FunctionCallbackInfo<v8::Value>& args) {
  auto isolate = args.GetIsolate();
  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  args.GetReturnValue().Set(self->ToObjectImpl(isolate));
}

Local<Object> Subscriber::SubscriberConfig::ToObjectImpl(Isolate* isolate) const {
  assert(isolate);
  auto context = isolate->GetCurrentContext();
  auto obj = Object::New(isolate);
  auto bixby = Object::New(isolate);
  obj->Set(context, String::NewFromUtf8(isolate, "Bixby"), bixby).FromJust();
  bixby->Set(context, String::NewFromUtf8(isolate, "host"),
                      String::NewFromUtf8(isolate, bixby_endpoint_.host().c_str())).FromJust();
  bixby->Set(context, String::NewFromUtf8(isolate, "port"),
                      Uint32::NewFromUnsigned(isolate, bixby_endpoint_.port())).FromJust();
  auto allocator = Object::New(isolate);
  obj->Set(context, String::NewFromUtf8(isolate, "Allocator"), allocator).FromJust();
  allocator->Set(context, String::NewFromUtf8(isolate, "host"),
                      String::NewFromUtf8(isolate, allocator_endpoint_.host().c_str())).FromJust();
  allocator->Set(context, String::NewFromUtf8(isolate, "port"),
                      Uint32::NewFromUnsigned(isolate, allocator_endpoint_.port())).FromJust();
  allocator->Set(context, String::NewFromUtf8(isolate, "loc"),
                      String::NewFromUtf8(isolate, alloc_location_.c_str())).FromJust();
  auto notifier = Object::New(isolate);
  obj->Set(context, String::NewFromUtf8(isolate, "Notifier"), notifier).FromJust();
  notifier->Set(context, String::NewFromUtf8(isolate, "host"),
                      String::NewFromUtf8(isolate, notifier_endpoint_.host().c_str())).FromJust();
  notifier->Set(context, String::NewFromUtf8(isolate, "port"),
                      Uint32::NewFromUnsigned(isolate, notifier_endpoint_.port())).FromJust();
  notifier->Set(context, String::NewFromUtf8(isolate, "tag"),
                      String::NewFromUtf8(isolate, tag_.c_str())).FromJust();
  notifier->Set(context, String::NewFromUtf8(isolate, "TLS"),
                      Boolean::New(isolate, notifier_use_tls_)).FromJust();
  notifier->Set(context, String::NewFromUtf8(isolate, "Cert"),
                      Boolean::New(isolate, notifier_cert_check_)).FromJust();
  obj->Set(context, String::NewFromUtf8(isolate, "Duration-ms"),
                      Int32::New(isolate, chrono::duration_cast<chrono::milliseconds>(duration_).count())).FromJust();
  obj->Set(context, String::NewFromUtf8(isolate, "UserId"),
                      String::NewFromUtf8(isolate, user_id_.c_str())).FromJust();
  obj->Set(context, String::NewFromUtf8(isolate, "StreamURL"),
                      String::NewFromUtf8(isolate, stream_url_.c_str())).FromJust();
  obj->Set(context, String::NewFromUtf8(isolate, "Cert"),
                      Boolean::New(isolate, cert_check_)).FromJust();
  obj->Set(context, String::NewFromUtf8(isolate, "Secret"),
                      String::NewFromUtf8(isolate, auth_secret_.c_str())).FromJust();
  obj->Set(context, String::NewFromUtf8(isolate, "FrameInfo"),
                      Boolean::New(isolate, print_frame_info_)).FromJust();
  auto audio = Object::New(isolate);
  obj->Set(context, String::NewFromUtf8(isolate, "Audio"), audio).FromJust();
  audio->Set(context, String::NewFromUtf8(isolate, "Sink"),
                      String::NewFromUtf8(isolate, EastWood::AudioSinkString(audio_sink_).c_str())).FromJust();
  audio->Set(context, String::NewFromUtf8(isolate, "Param"),
                      String::NewFromUtf8(isolate, audio_sink_param_.c_str())).FromJust();
  auto video = Object::New(isolate);
  obj->Set(context, String::NewFromUtf8(isolate, "Video"), video).FromJust();
  video->Set(context, String::NewFromUtf8(isolate, "Sink"),
                      String::NewFromUtf8(isolate, EastWood::VideoSinkString(video_sink_).c_str())).FromJust();
  video->Set(context, String::NewFromUtf8(isolate, "Param"),
                      String::NewFromUtf8(isolate, video_sink_param_.c_str())).FromJust();
  auto retry = Object::New(isolate);
  obj->Set(context, String::NewFromUtf8(isolate, "Retry"), retry).FromJust();
  retry->Set(context, String::NewFromUtf8(isolate, "Max"),
                      Uint32::NewFromUnsigned(isolate, err_max_retries_)).FromJust();
  retry->Set(context, String::NewFromUtf8(isolate, "InitDelay-ms"),
                      Uint32::NewFromUnsigned(isolate, err_retry_init_delay_ms_)).FromJust();
  retry->Set(context, String::NewFromUtf8(isolate, "Progression"),
                      Number::New(isolate, err_retry_delay_progression_)).FromJust();
  return obj;
}


Subscriber::SubscriberConfig::SubscriberConfig() {
}

void Subscriber::SubscriberConfig::Init(Local<Object> exports) {
  Util::InitClass(exports, "SubscriberConfig", New, constructor,
            Util::Prototype("bixby"s, Bixby),
            Util::Prototype("bixbyAllocator"s, BixbyAllocator),
            Util::Prototype("streamNotifier"s, StreamNotifier),
            Util::Prototype("duration"s, Duration),
            Util::Prototype("userId"s, UserId),
            Util::Prototype("streamUrl"s, StreamUrl),
            Util::Prototype("certCheck"s, CertCheck),
            Util::Prototype("authSecret"s, AuthSecret),
            Util::Prototype("printFrameInfo"s, PrintFrameInfo),
            Util::Prototype("audioSink"s, AudioSink),
            Util::Prototype("videoSink"s, VideoSink),
            Util::Prototype("subscriptionErrorRetry"s, SubscriptionErrorRetry),
            Util::Prototype("toObject"s, ToObject));
}

Local<Object> Subscriber::SubscriberConfig::NewInstance(const FunctionCallbackInfo<Value>& args) {
  return Util::NewV8Instance(constructor, args);
}

void Subscriber::SubscriberConfig::New(const FunctionCallbackInfo<Value>& args) {
  AT_ADDON_NEW_CPP_INSTANCE(args, SubscriberConfig, );
}



void Subscriber::Init(Local<Object> exports) {
  Util::InitClass(exports, "Subscriber", New, constructor,
            Util::Prototype("configure"s, Configure),
            Util::Prototype("on"s, On),
            Util::Prototype("start"s, Start),
            Util::Prototype("stop"s, Stop)
  );

  SubscriberConfig::Init(exports);
}

Local<Object> Subscriber::NewInstance(const FunctionCallbackInfo<Value>& args) {
  return Util::NewV8Instance(constructor, args);
}

void Subscriber::New(const FunctionCallbackInfo<Value>& args) {
  AT_ADDON_NEW_CPP_INSTANCE(args, Subscriber, args);
}

}  // namespace ew
