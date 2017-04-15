/// @copyright © 2017 Airtime Media.  All rights reserved.

#include <vector>
#include <algorithm>
#include <utility>
#include <typeinfo>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "eastwood.h"
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


Persistent<Function> EastWood::constructor;
Persistent<Function> Subscriber::constructor;
Persistent<Function> Subscriber::SubscriberConfig::constructor;

at::Ptr<at::EventLoop> EastWood::event_loop_;


// --------------------------------------------

static boost::property_tree::ptree LoadLogPropertiesFiles(const string& log_props_file) {
  boost::property_tree::ptree log_props;
  if (!log_props_file.empty()) {
    boost::property_tree::ini_parser::read_ini(log_props_file, log_props);
  }
  return log_props;
}

static void SetLogLevel(EastWood::LogLevel level) {
  auto log_level = at::LogLevel::info;
  switch (level) {
    case EastWood::LogLevel_Debug:
      log_level = at::LogLevel::debug;
      break;
    case EastWood::LogLevel_Info:
      log_level = at::LogLevel::info;
      break;
    case EastWood::LogLevel_Warning:
      log_level = at::LogLevel::warning;
      break;
    case EastWood::LogLevel_Fatal:
      log_level = at::LogLevel::fatal;
      break;
    default:
      break;
  }
  at::SetInitialRootLogLevel(log_level);
}


EastWood::EastWood(LogLevel level, bool log_to_console, bool log_to_syslog, const string& log_props_file)
  : log_(at::log::keywords::channel = "addon.EastWood") {
  if (!event_loop_) {
    event_loop_ = at::EventLoop::New(max<uint32_t>(1, at::EventLoopImpl::GetDefaultNumThreads() - 2));
    // this allocates (# of cores - 2) threads
  
    boost::property_tree::ptree log_props = LoadLogPropertiesFiles(log_props_file);
    at::InitLogging(log_to_console, log_to_syslog, log_props);
    SetLogLevel(level);
  }
}

EastWood::~EastWood() {
}

string EastWood::AudioSinkString(AudioSinkType sink) {
  switch (sink) {
    case AudioSink_None:
      return "none";
    case AudioSink_File:
      return "file";
    case AudioSink_FFMpeg:
      return "ffmpeg";
    default:
      return "invalid" + to_string(static_cast<int>(sink));
  }
}

string EastWood::VideoSinkString(VideoSinkType sink) {
  switch (sink) {
    case VideoSink_None:
      return "none";
    case VideoSink_File:
      return "file";
    case VideoSink_FFMpeg:
      return "ffmpeg";
    default:
      return "invalid" + to_string(static_cast<int>(sink));
  }
}

void EastWood::Init(Local<Object> exports) {
  Util::InitClass(exports, "EastWood", New, constructor,
          Util::Prototype("createSubscriber"s, CreateSubscriber),

          AT_ADDON_CLASS_ENUM(LogLevel_Fatal),
          AT_ADDON_CLASS_ENUM(LogLevel_Error),
          AT_ADDON_CLASS_ENUM(LogLevel_Warning),
          AT_ADDON_CLASS_ENUM(LogLevel_Info),
          AT_ADDON_CLASS_ENUM(LogLevel_Debug),

          AT_ADDON_CLASS_ENUM(AudioSink_None),
          AT_ADDON_CLASS_ENUM(AudioSink_File),
          AT_ADDON_CLASS_ENUM(AudioSink_FFMpeg),
          AT_ADDON_CLASS_ENUM(VideoSink_None),
          AT_ADDON_CLASS_ENUM(VideoSink_File),
          AT_ADDON_CLASS_ENUM(VideoSink_FFMpeg)
  );

  Subscriber::Init(exports);
}

void EastWood::New(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  auto log_level = LogLevel_Info;
  if (!Util::CheckArgs(isolate, "EastWood", args, 3, 4,
      [&log_level, isolate](Local<Value> arg0, string& err_msg) {
        if (!Util::IsV8EnumType(arg0)) return false;
        log_level = static_cast<LogLevel>(Util::ToEnumType(isolate, arg0));
        if (log_level < LogLevel_Fatal || LogLevel_Debug < log_level) {
          err_msg = "Incorrect log level value " + to_string(log_level);
          return false;
        }
        return true;
      },
      [](Local<Value> arg1, string& err_msg) { return arg1->IsBoolean(); },
      [](Local<Value> arg2, string& err_msg) { return arg2->IsBoolean(); },
      [](Local<Value> arg3, string& err_msg) { return arg3->IsString(); })) return;

  auto log_to_console = Util::ToBool(isolate, args[1]);
  auto log_to_syslog = Util::ToBool(isolate, args[2]);
  auto log_props_file = ((3 < args.Length()) ? Util::ToString(args[3]) : ""s);
  Util::NewCppInstance<EastWood>(args, new EastWood(log_level, log_to_console, log_to_syslog, log_props_file));
}

void EastWood::CreateSubscriber(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  if (!Util::CheckArgs(isolate, "createSubscriber", args, 0, 0)) return;

  args.GetReturnValue().Set(Subscriber::NewInstance(args));
}

// --------------------------------------------

Subscriber::Subscriber(const FunctionCallbackInfo<Value>& args)
  : log_(at::log::keywords::channel = "addon.Subscriber")
  , config_(args.GetIsolate(), SubscriberConfig::NewInstance(args)) {
}

Subscriber::~Subscriber() {
  config_.Reset();
}

void Subscriber::Configure(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  if (!Util::CheckArgs(isolate, "configure", args, 0, 0)) return;
  Subscriber* self = Unwrap<Subscriber>(args.Holder());
  assert(self);
  args.GetReturnValue().Set(self->config_.Get(isolate));
}


void Subscriber::SubscriberConfig::Bixby(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  if (!Util::CheckArgs(isolate, "bixby", args, 2, 2,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsString(); },
    [](const Local<Value> arg1, string& err_msg) { return arg1->IsUint32(); })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->bixby_endpoint_ = at::Endpoint(Util::ToString(args[0]), Util::ToUint32(isolate, args[1]));
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::BixbyAllocator(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  if (!Util::CheckArgs(isolate, "bixbyAllocator", args, 3, 3,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsString(); },
    [](const Local<Value> arg1, string& err_msg) { return arg1->IsUint32(); },
    [](const Local<Value> arg2, string& err_msg) { return arg2->IsString(); })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->allocator_endpoint_ = at::Endpoint(Util::ToString(args[0]), Util::ToUint32(isolate, args[1]));
  self->alloc_location_ = Util::ToString(args[2]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::StreamNotifier(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  if (!Util::CheckArgs(isolate, "streamNotifier", args, 5, 5,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsString(); },
    [](const Local<Value> arg1, string& err_msg) { return arg1->IsUint32(); },
    [](const Local<Value> arg2, string& err_msg) { return arg2->IsString(); },
    [](const Local<Value> arg3, string& err_msg) { return arg3->IsBoolean(); },  
    [](const Local<Value> arg4, string& err_msg) { return arg4->IsBoolean(); })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->notifier_endpoint_ = at::Endpoint(Util::ToString(args[0]), Util::ToUint32(isolate, args[1]));
  self->tag_ = Util::ToString(args[2]);
  self->notifier_use_tls_ = Util::ToBool(isolate, args[3]);
  self->notifier_cert_check_ = Util::ToBool(isolate, args[4]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::Duration(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  at::Duration dur = 0s;
  if (!Util::CheckArgs(isolate, "duration", args, 1, 1,
    [&dur](const Local<Value> arg0, string& err_msg) {
      if (!arg0->IsString()) return false;
      try {
        dur = at::eastwood::DurationFromString(Util::ToString(arg0));
      } catch (const exception& ex) {
        return false;
      }
      return true;
    })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->duration_ = dur;
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::UserId(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  if (!Util::CheckArgs(isolate, "userId", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsString(); })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->user_id_ = Util::ToString(args[0]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::StreamUrl(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  if (!Util::CheckArgs(isolate, "streamUrl", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsString(); })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->stream_url_ = Util::ToString(args[0]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::CertCheck(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  if (!Util::CheckArgs(isolate, "certCheck", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsBoolean(); })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->cert_check_ = Util::ToBool(isolate, args[0]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::AuthSecret(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  if (!Util::CheckArgs(isolate, "authSecret", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsString(); })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->auth_secret_ = Util::ToString(args[0]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::PrintFrameInfo(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  if (!Util::CheckArgs(isolate, "printFrameInfo", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsBoolean(); })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->print_frame_info_ = Util::ToBool(isolate, args[0]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::AudioSink(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  auto sink = EastWood::AudioSink_None;
  if (!Util::CheckArgs(isolate, "audioSink", args, 1, 2,
        [&args, isolate, &sink](const Local<Value> arg0, string& err_msg) {
          if (!Util::IsV8EnumType(arg0)) return false;
          sink = static_cast<EastWood::AudioSinkType>(Util::ToEnumType(isolate, arg0));
          if ((sink == EastWood::AudioSink_File
            || sink == EastWood::AudioSink_FFMpeg)) {
            if (2 == args.Length()) return true;
            err_msg = "Need sink param";
            return false;
          } else if (sink == EastWood::AudioSink_None) {
            if (1 == args.Length()) return true;
            err_msg = "Sink param should not be given";
            return false;
          } else {
            return false;
          }
        },
        [](const Local<Value> arg1, string& err_msg) { return arg1->IsString(); })) {
    return;
  }

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->audio_sink_ = sink;
  if (2 == args.Length()) {
    self->audio_sink_param_ = Util::ToString(args[1]);
  }
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::VideoSink(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  auto sink = EastWood::VideoSink_None;
  if (!Util::CheckArgs(isolate, "videoSink", args, 1, 2,
        [&args, isolate, &sink](const Local<Value> arg0, string& err_msg) {
          if (!Util::IsV8EnumType(arg0)) return false;
          sink = static_cast<EastWood::VideoSinkType>(Util::ToEnumType(isolate, arg0));
          if ((sink == EastWood::VideoSink_File
            || sink == EastWood::VideoSink_FFMpeg)) {
            if (2 == args.Length()) return true;
            err_msg = "Need sink param";
            return false;
          } else if (sink == EastWood::VideoSink_None) {
            if (1 == args.Length()) return true;
            err_msg = "Sink param should not be given";
            return false;
          } else {
            return false;
          }
        },
        [](const Local<Value> arg1, string& err_msg) { return arg1->IsString(); })) {
      return;
  }

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->video_sink_ = sink;
  if (2 == args.Length()) {
    self->video_sink_param_ = Util::ToString(args[1]);
  }
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::SubscriptionErrorRetry(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  if (!Util::CheckArgs(isolate, "subscriptionErrorRetry", args, 3, 3,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsUint32(); },
    [](const Local<Value> arg1, string& err_msg) { return arg1->IsUint32(); },
    [](const Local<Value> arg2, string& err_msg) { return arg2->IsNumber(); })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->err_max_retries_ = Util::ToUint32(isolate, args[0]);
  self->err_retry_init_delay_ms_ = Util::ToUint32(isolate, args[1]);;
  self->err_retry_delay_progression_ = Util::ToDouble(isolate, args[2]);;
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::On(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();

  if (!Util::CheckArgs(isolate, "on", args, 2, 2,
    [](const Local<Value> arg0, string& err_msg) {
      if (!arg0->IsString()) return false;
      auto event = Util::ToString(arg0);
      return ("started" == event
           || "ended" == event
           || "error" == event);
    },
    [](const Local<Value> arg1, string& err_msg) { return arg1->IsFunction(); })) return;

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
  if (!Util::CheckArgs(isolate, "start", args, 0, 0)) return;
  Subscriber* self = Unwrap<Subscriber>(args.Holder());
  assert(self);

  SubscriberConfig* config = Unwrap<SubscriberConfig>(self->config_.Get(isolate));
  assert(config);
  config->VerifyAllConfig(args);

  // TODO(Art): start!
}

void Subscriber::Stop(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  if (!Util::CheckArgs(isolate, "stop", args, 0, 0)) return;
  Subscriber* self = Unwrap<Subscriber>(args.Holder());
  assert(self);

  // TODO(Art): stop!
}

void Subscriber::SubscriberConfig::Verify(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  if (!Util::CheckArgs(isolate, "verify", args, 0, 0)) return;
  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->VerifyAllConfig(args);
}

void Subscriber::SubscriberConfig::VerifyAllConfig(const FunctionCallbackInfo<Value>& args) const {
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
  // no specific check
}

void Subscriber::SubscriberConfig::VerifyVideoSinkConfig(const FunctionCallbackInfo<Value>& args) const {
  // no specific check
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
  if ((audio_sink_ == EastWood::AudioSink_FFMpeg
    && (video_sink_ != EastWood::VideoSink_FFMpeg
     && video_sink_ != EastWood::VideoSink_None))
   || ((audio_sink_ != EastWood::AudioSink_FFMpeg
     && audio_sink_ != EastWood::AudioSink_None)
    && video_sink_ == EastWood::VideoSink_FFMpeg)) {
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
  obj->Set(context, String::NewFromUtf8(isolate, "bixby"), bixby).FromJust();
  bixby->Set(context, String::NewFromUtf8(isolate, "host"),
                      String::NewFromUtf8(isolate, bixby_endpoint_.host().c_str())).FromJust();
  bixby->Set(context, String::NewFromUtf8(isolate, "port"),
                      Uint32::NewFromUnsigned(isolate, bixby_endpoint_.port())).FromJust();
  auto allocator = Object::New(isolate);
  obj->Set(context, String::NewFromUtf8(isolate, "allocator"), allocator).FromJust();
  allocator->Set(context, String::NewFromUtf8(isolate, "host"),
                      String::NewFromUtf8(isolate, allocator_endpoint_.host().c_str())).FromJust();
  allocator->Set(context, String::NewFromUtf8(isolate, "port"),
                      Uint32::NewFromUnsigned(isolate, allocator_endpoint_.port())).FromJust();
  allocator->Set(context, String::NewFromUtf8(isolate, "loc"),
                      String::NewFromUtf8(isolate, alloc_location_.c_str())).FromJust();
  auto notifier = Object::New(isolate);
  obj->Set(context, String::NewFromUtf8(isolate, "notifier"), notifier).FromJust();
  notifier->Set(context, String::NewFromUtf8(isolate, "host"),
                      String::NewFromUtf8(isolate, notifier_endpoint_.host().c_str())).FromJust();
  notifier->Set(context, String::NewFromUtf8(isolate, "port"),
                      Uint32::NewFromUnsigned(isolate, notifier_endpoint_.port())).FromJust();
  notifier->Set(context, String::NewFromUtf8(isolate, "tag"),
                      String::NewFromUtf8(isolate, tag_.c_str())).FromJust();
  notifier->Set(context, String::NewFromUtf8(isolate, "tls"),
                      Boolean::New(isolate, notifier_use_tls_)).FromJust();
  notifier->Set(context, String::NewFromUtf8(isolate, "cert"),
                      Boolean::New(isolate, notifier_cert_check_)).FromJust();
  obj->Set(context, String::NewFromUtf8(isolate, "duration_ms"),
                      Int32::New(isolate, chrono::duration_cast<chrono::milliseconds>(duration_).count())).FromJust();
  obj->Set(context, String::NewFromUtf8(isolate, "userId"),
                      String::NewFromUtf8(isolate, user_id_.c_str())).FromJust();
  obj->Set(context, String::NewFromUtf8(isolate, "streamURL"),
                      String::NewFromUtf8(isolate, stream_url_.c_str())).FromJust();
  obj->Set(context, String::NewFromUtf8(isolate, "cert"),
                      Boolean::New(isolate, cert_check_)).FromJust();
  obj->Set(context, String::NewFromUtf8(isolate, "secret"),
                      String::NewFromUtf8(isolate, auth_secret_.c_str())).FromJust();
  obj->Set(context, String::NewFromUtf8(isolate, "frameInfo"),
                      Boolean::New(isolate, print_frame_info_)).FromJust();
  auto audio = Object::New(isolate);
  obj->Set(context, String::NewFromUtf8(isolate, "audio"), audio).FromJust();
  audio->Set(context, String::NewFromUtf8(isolate, "sink"),
                      String::NewFromUtf8(isolate, EastWood::AudioSinkString(audio_sink_).c_str())).FromJust();
  audio->Set(context, String::NewFromUtf8(isolate, "param"),
                      String::NewFromUtf8(isolate, audio_sink_param_.c_str())).FromJust();
  auto video = Object::New(isolate);
  obj->Set(context, String::NewFromUtf8(isolate, "video"), video).FromJust();
  video->Set(context, String::NewFromUtf8(isolate, "sink"),
                      String::NewFromUtf8(isolate, EastWood::VideoSinkString(video_sink_).c_str())).FromJust();
  video->Set(context, String::NewFromUtf8(isolate, "param"),
                      String::NewFromUtf8(isolate, video_sink_param_.c_str())).FromJust();
  auto retry = Object::New(isolate);
  obj->Set(context, String::NewFromUtf8(isolate, "retry"), retry).FromJust();
  retry->Set(context, String::NewFromUtf8(isolate, "max"),
                      Uint32::NewFromUnsigned(isolate, err_max_retries_)).FromJust();
  retry->Set(context, String::NewFromUtf8(isolate, "initDelay_ms"),
                      Uint32::NewFromUnsigned(isolate, err_retry_init_delay_ms_)).FromJust();
  retry->Set(context, String::NewFromUtf8(isolate, "progression"),
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
            Util::Prototype("verify"s, Verify),
            Util::Prototype("toObject"s, ToObject));
}

Local<Object> Subscriber::SubscriberConfig::NewInstance(const FunctionCallbackInfo<Value>& args) {
  return Util::NewV8Instance(constructor, args);
}

void Subscriber::SubscriberConfig::New(const FunctionCallbackInfo<Value>& args) {
  Util::NewCppInstance(args, new SubscriberConfig());
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
  Util::NewCppInstance(args, new Subscriber(args));
}

}  // namespace ew
