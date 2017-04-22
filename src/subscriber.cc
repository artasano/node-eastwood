/// @copyright © 2017 Airtime Media.  All rights reserved.

#include <algorithm>
#include <utility>
#include <iostream>
#include <typeinfo>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <uv.h>

#include "subscriber.h"
#include "addon_util.h"

#include "mediacore/base/exception.h"
#include "eastwood/common/time.h"
#include "eastwood/common/string_tokenizer.h"

#include "eastwood/sink/stream_sink_factory.h"
#include "eastwood/ffmpeg/ffmpeg_stream_sink_factory.h"

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
using v8::Undefined;
using v8::Null;
using v8::Exception;
using v8::PropertyAttribute;

using namespace std;
using namespace string_literals;
using namespace chrono_literals;

using at::node_addon::Util;

using at::eastwood::SubscriberFacade;

Persistent<Function> Subscriber::constructor;
Persistent<Function> Subscriber::SubscriberConfig::constructor;

// --------------------------------------------

Subscriber::Subscriber(const FunctionCallbackInfo<Value>& args)
  : log_(at::log::keywords::channel = "addon.Subscriber")
  , config_(args.GetIsolate(), SubscriberConfig::NewInstance(args)) {
}

Subscriber::~Subscriber() {
  config_.Reset();

// TODO(Art): temp
std::cout << "~Subscriber\n";
}

void Subscriber::Configuration(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  if (!Util::CheckArgs("configuration", args, 0, 0)) return;
  Subscriber* self = Unwrap<Subscriber>(args.Holder());
  assert(self);
  args.GetReturnValue().Set(self->config_.Get(isolate));
}


void Subscriber::SubscriberConfig::Bixby(const FunctionCallbackInfo<Value>& args) {
  auto host = ""s;
  auto port = 0u;
  if (!Util::CheckArgs("bixby", args, 2, 2,
    [&host](const Local<Value> arg0, string& err_msg) {
      if (!arg0->IsString()) return false;
      host = Util::ToString(arg0);
      if (host.empty()) {
        err_msg = "hostname cannot be empty";
        return false;
      }
      return true;
    },
    [&port](const Local<Value> arg1, string& err_msg) {
      if (!arg1->IsUint32()) return false;
      port = Util::ToUint32(arg1);
      if (0 == port) {
        err_msg = "port number cannot be zero";
        return false;
      }
      return true;
    })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->config_.bixby_endpoint = at::Endpoint(host, port);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::BixbyAllocator(const FunctionCallbackInfo<Value>& args) {
  auto host = ""s;
  auto port = 0u;
  auto loc = ""s;
  if (!Util::CheckArgs("bixbyAllocator", args, 3, 3,
    [&host](const Local<Value> arg0, string& err_msg) {
      if (!arg0->IsString()) return false;
      host = Util::ToString(arg0);
      if (host.empty()) {
        err_msg = "hostname cannot be empty";
        return false;
      }
      return true;
    },
    [&port](const Local<Value> arg1, string& err_msg) {
      if (!arg1->IsUint32()) return false;
      port = Util::ToUint32( arg1);
      if (0 == port) {
        err_msg = "port number cannot be zero";
        return false;
      }
      return true;
    },
    [&loc](const Local<Value> arg2, string& err_msg) {
      if (!arg2->IsString()) return false;
      loc = Util::ToString(arg2);
      if (loc.empty()) {
        err_msg = "location cannot be empty";
        return false;
      }
      return true;
    })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->config_.bixby_allocator_endpoint = at::Endpoint(host, port);
  self->config_.alloc_location = loc;
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::StreamNotifier(const FunctionCallbackInfo<Value>& args) {
  auto host = ""s;
  auto port = 0u;
  auto tag = ""s;
  if (!Util::CheckArgs("streamNotifier", args, 5, 5,
    [&host](const Local<Value> arg0, string& err_msg) {
      if (!arg0->IsString()) return false;
      host = Util::ToString(arg0);
      if (host.empty()) {
        err_msg = "hostname cannot be empty";
        return false;
      }
      return true;
    },
    [&port](const Local<Value> arg1, string& err_msg) {
      if (!arg1->IsUint32()) return false;
      port = Util::ToUint32(arg1);
      if (0 == port) {
        err_msg = "port number cannot be zero";
        return false;
      }
      return true;
    },
    [&tag](const Local<Value> arg2, string& err_msg) {
      if (!arg2->IsString()) return false;
      tag = Util::ToString(arg2);
      if (tag.empty()) {
        err_msg = "tag cannot be empty";
        return false;
      }
      return true;
    },
    [](const Local<Value> arg3, string& err_msg) { return arg3->IsBoolean(); },
    [](const Local<Value> arg4, string& err_msg) { return arg4->IsBoolean(); })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->config_.notifier_endpoint = at::Endpoint(host, port);
  self->config_.tag = tag;
  self->config_.use_tls_for_notifier = Util::ToBool(args[3]);
  self->config_.no_notifier_cert_check = !Util::ToBool(args[4]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::Duration(const FunctionCallbackInfo<Value>& args) {
  at::Duration dur = 0s;
  if (!Util::CheckArgs("duration", args, 1, 1,
    [&dur](const Local<Value> arg0, string& err_msg) {
      if (!arg0->IsString()) return false;
      try {
        dur = at::eastwood::DurationFromString(Util::ToString(arg0));
      } catch (const exception& ex) {
        return false;
      }
      if (dur <= 0s) {
        err_msg = "duration must be longer than zero";
        return false;
      }
      return true;
    })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->config_.duration = dur;
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::UserId(const FunctionCallbackInfo<Value>& args) {
  auto uid = ""s;
  if (!Util::CheckArgs("userId", args, 1, 1,
    [&uid](const Local<Value> arg0, string& err_msg) {
      if (!arg0->IsString()) return false;
      uid = Util::ToString(arg0);
      if (uid.empty()) {
        err_msg = "user id cannot be empty";
        return false;
      }
      return true;
    })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->config_.user_id = uid;
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::StreamUrl(const FunctionCallbackInfo<Value>& args) {
  auto url = ""s;
  if (!Util::CheckArgs("streamUrl", args, 1, 1,
    [&url](const Local<Value> arg0, string& err_msg) {
      if (!arg0->IsString()) return false;
      url = Util::ToString(arg0);
      if (url.empty()) {
        err_msg = "stream URL cannot be empty";
        return false;
      }
      return true;
    })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->config_.stream_url = url;
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::CertCheck(const FunctionCallbackInfo<Value>& args) {
  if (!Util::CheckArgs("certCheck", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsBoolean(); })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->config_.no_cert_check = !Util::ToBool(args[0]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::AuthSecret(const FunctionCallbackInfo<Value>& args) {

  if (!Util::CheckArgs("authSecret", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsString(); })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->config_.auth_secret = Util::ToString(args[0]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::PrintFrameInfo(const FunctionCallbackInfo<Value>& args) {

  if (!Util::CheckArgs("printFrameInfo", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsBoolean(); })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->config_.print_frame_info = Util::ToBool(args[0]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::AudioSink(const FunctionCallbackInfo<Value>& args) {
  auto sink = EastWood::AudioSink_None;
  auto filename = ""s;
  if (!Util::CheckArgs("audioSink", args, 1, 2,
        [&args, &sink](const Local<Value> arg0, string& err_msg) {
          if (!Util::IsV8EnumType(arg0)) return false;
          sink = static_cast<EastWood::AudioSinkType>(Util::ToEnumType(arg0));
          if (sink == EastWood::AudioSink_File) {
            if (2 == args.Length()) return true;
            err_msg = "Need sink filename";
            return false;
          } else if (sink == EastWood::AudioSink_None) {
            if (1 == args.Length()) return true;
            err_msg = "Sink param should not be given";
            return false;
          } else {
            return false;
          }
        },
        [&filename](const Local<Value> arg1, string& err_msg) {
          if (!arg1->IsString()) return false;
          filename = Util::ToString(arg1);
          if (filename.empty()) {
            err_msg = "Sink filename cannot be empty";
            return false;
          }
          return true;
        })) {
    return;
  }

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->audio_sink_ = sink;
  self->audio_sink_filename_ = filename;
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::VideoSink(const FunctionCallbackInfo<Value>& args) {
  auto sink = EastWood::VideoSink_None;
  auto filename = ""s;
  if (!Util::CheckArgs("videoSink", args, 1, 2,
        [&args, &sink](const Local<Value> arg0, string& err_msg) {
          if (!Util::IsV8EnumType(arg0)) return false;
          sink = static_cast<EastWood::VideoSinkType>(Util::ToEnumType(arg0));
          if (sink == EastWood::VideoSink_File) {
            if (2 == args.Length()) return true;
            err_msg = "Need sink filename";
            return false;
          } else if (sink == EastWood::VideoSink_None) {
            if (1 == args.Length()) return true;
            err_msg = "Sink param should not be given";
            return false;
          } else {
            return false;
          }
        },
        [&filename](const Local<Value> arg1, string& err_msg) {
          if (!arg1->IsString()) return false;
          filename = Util::ToString(arg1);
          if (filename.empty()) {
            err_msg = "Sink filename cannot be empty";
            return false;
          }
          return true;
        })) {
      return;
  }

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->video_sink_ = sink;
  if (2 == args.Length()) {
    self->video_sink_filename_ = Util::ToString(args[1]);
  } else {
    self->video_sink_filename_ = "";
  }
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::FFMpegSink(const FunctionCallbackInfo<Value>& args) {
  auto output = ""s;
  if (!Util::CheckArgs("ffmpegSink", args, 2, 2,
        [&output](const Local<Value> arg0, string& err_msg) {
          if (!arg0->IsString()) return false;
          output = Util::ToString(arg0);
          if (output.empty()) {
            err_msg = "output cannot be empty";
            return false;
          }
          return true;
        },
        [](const Local<Value> arg1, string& err_msg) { return arg1->IsString(); })) {
    return;
  }

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->ffmpeg_output_ = output;
  self->ffmpeg_param_ = Util::ToString(args[1]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::SubscriptionErrorRetry(const FunctionCallbackInfo<Value>& args) {
  auto max_retries = 0u;
  auto progression = 0.0;
  if (!Util::CheckArgs("subscriptionErrorRetry", args, 3, 3,
    [&max_retries](const Local<Value> arg0, string& err_msg) {
      if (!arg0->IsUint32()) return false;
      max_retries = Util::ToUint32(arg0);
      return true;
    },
    [](const Local<Value> arg1, string& err_msg) { return arg1->IsUint32(); },
    [&max_retries, &progression](const Local<Value> arg2, string& err_msg) {
      if (!arg2->IsNumber()) return false;
      if (0 < max_retries) {
        progression = Util::ToDouble(arg2);
      }
      if (progression < 1.0) {
        err_msg = "retry delay progression must be 1.0 or bigger";
        return false;
      }
      return true;
    })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->config_.err_max_retries = max_retries;
  self->config_.err_retry_delay_init_ms = Util::ToUint32(args[1]);;
  self->config_.err_retry_delay_progression = progression;
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::On(const FunctionCallbackInfo<Value>& args) {
  if (!Util::CheckArgs("on", args, 2, 2,
    [](const Local<Value> arg0, string& err_msg) {
      if (!arg0->IsString()) return false;
      auto event = Util::ToString(arg0);
      return ("error" == event);
    },
    [](const Local<Value> arg1, string& err_msg) { return arg1->IsFunction(); })) return;

  Subscriber* self = Unwrap<Subscriber>(args.Holder());
  assert(self);

  self->error_event_.AddListener(Local<Function>::Cast(args[1]));
}

void Subscriber::Start(const FunctionCallbackInfo<Value>& args) {
  if (!Util::CheckArgs("start", args, 0, 0)) return;
  Subscriber* self = Unwrap<Subscriber>(args.Holder());
  assert(self);

  AT_LOG_INFO(self->log_, "Starting");

  SubscriberConfig* config = Unwrap<SubscriberConfig>(self->config_.Get(args.GetIsolate()));
  assert(config);
  if (!config->VerifyConfigIntegrity(args)) {
    // thrown
    return;
  }

  self->CreateSinks(*config);

  // Lazy init of facade
  if (!self->facade_) {
    self->facade_ = make_unique<SubscriberFacade>(EastWood::event_loop, move(config->config_));
  }
  self->facade_->Start();
  AT_LOG_INFO(self->log_, "Started");
}

void Subscriber::CreateSinks(SubscriberConfig& config) {
  // a/v sink integrity has been checked already, so just checking one of them is sufficient here.
  if (!config.ffmpeg_output_.empty()) {
    CreateFFMpegSinks(config);
  } else {
    CreateRegularSinks(config);
  }
}

void Subscriber::CreateRegularSinks(SubscriberConfig& config) {
  at::eastwood::AudioSinkConfig audio_config;
  switch (config.audio_sink_) {
    case EastWood::AudioSink_None:
      audio_config.audio_sink = at::eastwood::audio_sink_t::kAudioSinkNone;
      break;
    case EastWood::AudioSink_File:
      audio_config.audio_sink = at::eastwood::audio_sink_t::kAudioSinkFileRaw;
      audio_config.filename = config.audio_sink_filename_;
      break;
    default:  // undefined
      // cannot happen
      assert(false);
      break;
  }

  at::eastwood::VideoSinkConfig video_config;
  switch (config.video_sink_) {
    case EastWood::VideoSink_None:
      video_config.video_sink = at::eastwood::video_sink_t::kVideoSinkNone;
      break;
    case EastWood::VideoSink_File:
      video_config.video_sink = at::eastwood::video_sink_t::kVideoSinkFileRaw;
      video_config.filename = config.video_sink_filename_;
      break;
    default:  // undefined
      // cannot happen
      assert(false);
      break;
  }

  auto a_v_sinks = at::eastwood::StreamSinkFactory().CreateSinks(audio_config, video_config);
  config.config_.audio_sink = move(a_v_sinks.first);
  config.config_.video_sink = move(a_v_sinks.second);
}

static pair<string, string> SplitKeyValue(const string& token, char separator) {
  auto pos = token.find(separator);
  if (pos < token.size()) {
    return make_pair(token.substr(0, pos), token.substr(pos+1));
  } else {
    return make_pair(token, ""s);
  }
}

void Subscriber::CreateFFMpegSinks(SubscriberConfig& config) {
  using at::eastwood::FFmpegStreamSinkFactory;
  FFmpegStreamSinkFactory::OptionMap opts;
  auto params = at::eastwood::StringTokenizer::Tokenize(config.ffmpeg_param_, ' ');
  for (const auto& param : params) {
    auto key_val = SplitKeyValue(param, '=');  // key-value separator
    opts[key_val.first] = key_val.second;
  }
  auto factory = FFmpegStreamSinkFactory(config.ffmpeg_output_, opts);

  // register error handler
  factory.RegisterRoomIdleTimeoutHandler([this]() {
    NotifyError(kErrorIdleTimeout);
  });
  factory.RegisterStreamOutputFailureHandler([this]() {
    NotifyError(kErrorOutputFailure);
  });

  auto a_v_sinks = factory.CreateSinks(at::eastwood::AudioSinkConfig(), at::eastwood::VideoSinkConfig());
  config.config_.audio_sink = move(a_v_sinks.first);
  config.config_.video_sink = move(a_v_sinks.second);
}

void Subscriber::NotifyError(const string& err) {
  AT_LOG_INFO(log_, "Notifying error: " << err);
  vector<Local<Value>> args;
  if (err.empty()) {
    args.emplace_back(Undefined(Isolate::GetCurrent()));
  } else {
    args.emplace_back(Exception::Error(Util::ToLocalString(err)));
  }
  error_event_.Emit(args);
}

void Subscriber::Stop(const FunctionCallbackInfo<Value>& args) {
  v8::HandleScope scope(args.GetIsolate());

  if (!Util::CheckArgs("stop", args, 0, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsFunction(); }
  )) return;
  Subscriber* self = Unwrap<Subscriber>(args.Holder());
  assert(self);

  AT_LOG_INFO(self->log_, "Stopping");

  self->error_event_.Stop();

  auto isolate = args.GetIsolate();
  if (1 == args.Length()) {
    self->stop_callback_.SetCallbackFn(Local<Function>::Cast(args[0]));
  } else {
    self->stop_callback_.SetCallbackFn(
                               Function::New(isolate->GetCurrentContext(),
                                   [](const FunctionCallbackInfo<Value>& args){}).ToLocalChecked());
  }

  if (!self->facade_) {
    // Stopped before Start.
    // calling on_ended listeners with error : that's the same as subscriber's behavior.
    vector<Local<Value>> args;
    args.emplace_back(Util::ToLocalValue(false));
    self->stop_callback_.Call(args);
    return;
  }

  self->facade_->Stop()->on_result([self](exception_ptr ex, bool result) {
    // TODO(Art): write
    // if (ex) {
    //   AT_LOG_ERROR(self->log_, "Stop callback with exception: " << ex);
    //   auto argc = 1;
    //   Local<Value> argv[] = { Util::ToLocalValue(false) };
    //   self->stop_callback_->Call(context, Null(Isolate::GetCurrent()), argc, argv).IsEmpty();
    //   return;
    // }
    // // TODO: callback
  });
}

void Subscriber::SubscriberConfig::Verify(const FunctionCallbackInfo<Value>& args) {
  if (!Util::CheckArgs("verify", args, 0, 0)) return;
  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->VerifyConfigIntegrity(args);
}

bool Subscriber::SubscriberConfig::VerifyConfigIntegrity(const FunctionCallbackInfo<Value>& args) const {
  auto err = ""s;
  if (at::Endpoint() == config_.bixby_endpoint && at::Endpoint() == config_.bixby_allocator_endpoint) {
    err += "Need either Bixby endpoint or Allocator endpoint\n";
  }
  if (at::Endpoint() != config_.bixby_endpoint && at::Endpoint() != config_.bixby_allocator_endpoint) {
    err += "Bixby endpoint and Allocator endpoint are mutually exclusive\n";
  }
  if (at::Endpoint() == config_.notifier_endpoint && config_.stream_url.empty()) {
    err += "Need either Stream notifier endpoint or Stream URL\n";
  }
  if (at::Endpoint() != config_.notifier_endpoint && !config_.stream_url.empty()) {
    err += "Stream notifier endpoint and Stream URL are mutually exclusive\n";
  }
  if (ffmpeg_output_.empty()
   && ((video_sink_ == EastWood::VideoSink_Undefined) && (audio_sink_ == EastWood::AudioSink_Undefined))) {
    err += "Need sink\n";
  }
  if (!ffmpeg_output_.empty()
   && ((video_sink_ != EastWood::VideoSink_Undefined) || (audio_sink_ != EastWood::AudioSink_Undefined))) {
    err += "Regular sink and FFMpeg sink are mutually exclusive\n";
  }
  if (config_.duration == at::Duration()) {
    err += "Need duration\n";
  }
  if (config_.user_id.empty()) {
    err += "Need user id\n";
  }

  if (!err.empty()) {
    AT_LOG_WARNING(log_, err);
    Util::ThrowException(args, Exception::Error, err);
    return false;
  } else {
    AT_LOG_INFO(log_, "Configuration verified OK");
    return true;
  }
}

void Subscriber::SubscriberConfig::ToObject(const v8::FunctionCallbackInfo<v8::Value>& args) {
  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  args.GetReturnValue().Set(self->ToObjectImpl());
}

Local<Object> Subscriber::SubscriberConfig::ToObjectImpl() const {
  auto isolate = Isolate::GetCurrent();
  auto context = isolate->GetCurrentContext();
  auto obj = Object::New(isolate);
  auto bixby = Object::New(isolate);
  obj->Set(context, Util::ToLocalString("bixby"), bixby).FromJust();
  bixby->Set(context, Util::ToLocalString("host"),
                      Util::ToLocalString(config_.bixby_endpoint.host())).FromJust();
  bixby->Set(context, Util::ToLocalString("port"),
                      Util::ToLocalInteger(config_.bixby_endpoint.port())).FromJust();
  auto allocator = Object::New(isolate);
  obj->Set(context, Util::ToLocalString("allocator"), allocator).FromJust();
  allocator->Set(context, Util::ToLocalString("host"),
                      Util::ToLocalString(config_.bixby_allocator_endpoint.host())).FromJust();
  allocator->Set(context, Util::ToLocalString("port"),
                      Util::ToLocalInteger(config_.bixby_allocator_endpoint.port())).FromJust();
  allocator->Set(context, Util::ToLocalString("loc"),
                      Util::ToLocalString(config_.alloc_location)).FromJust();
  auto notifier = Object::New(isolate);
  obj->Set(context, Util::ToLocalString("notifier"), notifier).FromJust();
  notifier->Set(context, Util::ToLocalString("host"),
                      Util::ToLocalString(config_.notifier_endpoint.host())).FromJust();
  notifier->Set(context, Util::ToLocalString("port"),
                      Util::ToLocalInteger(config_.notifier_endpoint.port())).FromJust();
  notifier->Set(context, Util::ToLocalString("tag"),
                      Util::ToLocalString(config_.tag)).FromJust();
  notifier->Set(context, Util::ToLocalString("tls"),
                      Util::ToLocalBoolean(config_.use_tls_for_notifier)).FromJust();
  notifier->Set(context, Util::ToLocalString("cert"),
                      Util::ToLocalBoolean(!config_.no_notifier_cert_check)).FromJust();
  obj->Set(context, Util::ToLocalString("duration_ms"),
                      Util::ToLocalInteger(chrono::duration_cast<chrono::milliseconds>(config_.duration).count())).FromJust();
  obj->Set(context, Util::ToLocalString("userId"),
                      Util::ToLocalString(config_.user_id)).FromJust();
  obj->Set(context, Util::ToLocalString("streamURL"),
                      Util::ToLocalString(config_.stream_url)).FromJust();
  obj->Set(context, Util::ToLocalString("cert"),
                      Util::ToLocalBoolean(!config_.no_cert_check)).FromJust();
  obj->Set(context, Util::ToLocalString("secret"),
                      Util::ToLocalString(config_.auth_secret)).FromJust();
  obj->Set(context, Util::ToLocalString("frameInfo"),
                      Util::ToLocalBoolean(config_.print_frame_info)).FromJust();
  if (!ffmpeg_output_.empty()) {
    auto ffmpeg = Object::New(isolate);
    obj->Set(context, Util::ToLocalString("ffmpeg"), ffmpeg).FromJust();
    ffmpeg->Set(context, Util::ToLocalString("output"),
                         Util::ToLocalString(ffmpeg_output_)).FromJust();
    ffmpeg->Set(context, Util::ToLocalString("params"),
                         Util::ToLocalString(ffmpeg_param_)).FromJust();
  } else {
    auto audio = Object::New(isolate);
    obj->Set(context, Util::ToLocalString("audio"), audio).FromJust();
    audio->Set(context, Util::ToLocalString("sink"),
                        Util::ToLocalString(EastWood::AudioSinkString(audio_sink_))).FromJust();
    audio->Set(context, Util::ToLocalString("filename"),
                        Util::ToLocalString(audio_sink_filename_)).FromJust();
    auto video = Object::New(isolate);
    obj->Set(context, Util::ToLocalString("video"), video).FromJust();
    video->Set(context, Util::ToLocalString("sink"),
                        Util::ToLocalString(EastWood::VideoSinkString(video_sink_))).FromJust();
    video->Set(context, Util::ToLocalString("filename"),
                        Util::ToLocalString(video_sink_filename_)).FromJust();
  }
  auto retry = Object::New(isolate);
  obj->Set(context, Util::ToLocalString("retry"), retry).FromJust();
  retry->Set(context, Util::ToLocalString("max"),
                      Util::ToLocalInteger(config_.err_max_retries)).FromJust();
  retry->Set(context, Util::ToLocalString("initDelay_ms"),
                      Util::ToLocalInteger(config_.err_retry_delay_init_ms)).FromJust();
  retry->Set(context, Util::ToLocalString("progression"),
                      Util::ToLocalNumber(config_.err_retry_delay_progression)).FromJust();
  return obj;
}


Subscriber::SubscriberConfig::SubscriberConfig() 
  : log_(at::log::keywords::channel = "addon.SubscriberConfig") {
}

void Subscriber::SubscriberConfig::Init(Local<Object> exports) {
  Util::InitClass(exports, "SubscriberConfig", New, constructor,
            Util::Prototype("bixby", Bixby),
            Util::Prototype("bixbyAllocator", BixbyAllocator),
            Util::Prototype("streamNotifier", StreamNotifier),
            Util::Prototype("duration", Duration),
            Util::Prototype("userId", UserId),
            Util::Prototype("streamUrl", StreamUrl),
            Util::Prototype("certCheck", CertCheck),
            Util::Prototype("authSecret", AuthSecret),
            Util::Prototype("printFrameInfo", PrintFrameInfo),
            Util::Prototype("audioSink", AudioSink),
            Util::Prototype("videoSink", VideoSink),
            Util::Prototype("ffmpegSink", FFMpegSink),
            Util::Prototype("subscriptionErrorRetry", SubscriptionErrorRetry),
            Util::Prototype("verify", Verify),
            Util::Prototype("toObject", ToObject));
}

Local<Object> Subscriber::SubscriberConfig::NewInstance(const FunctionCallbackInfo<Value>& args) {
  return Util::NewV8Instance(constructor, args);
}

void Subscriber::SubscriberConfig::New(const FunctionCallbackInfo<Value>& args) {
  Util::NewCppInstance(args, new SubscriberConfig());
}



void Subscriber::Init(Local<Object> exports) {
  Util::InitClass(exports, "Subscriber", New, constructor,
            Util::Prototype("configuration", Configuration),
            Util::Prototype("on", On),
            Util::Prototype("start", Start),
            Util::Prototype("stop", Stop)
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
