/// @copyright © 2017 Airtime Media.  All rights reserved.

#include <algorithm>
#include <utility>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "subscriber.h"
#include "addon_util/addon_util.h"

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

using namespace at::node_addon;

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
}

void Subscriber::configuration(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  if (!CheckArgs("configuration", args, 0, 0)) return;
  Subscriber* self = Unwrap<Subscriber>(args.Holder());
  assert(self);
  args.GetReturnValue().Set(self->config_.Get(isolate));
}


void Subscriber::SubscriberConfig::bixby(const FunctionCallbackInfo<Value>& args) {
  auto host = ""s;
  auto port = 0u;
  if (!CheckArgs("bixby", args, 2, 2,
    [&host](const Local<Value> arg0, string& err_msg) {
      if (!arg0->IsString()) return false;
      host = ToString(arg0);
      if (host.empty()) {
        err_msg = "hostname cannot be empty";
        return false;
      }
      return true;
    },
    [&port](const Local<Value> arg1, string& err_msg) {
      if (!arg1->IsUint32()) return false;
      port = ToUint32(arg1);
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

void Subscriber::SubscriberConfig::bixbyAllocator(const FunctionCallbackInfo<Value>& args) {
  auto host = ""s;
  auto port = 0u;
  auto loc = ""s;
  if (!CheckArgs("bixbyAllocator", args, 3, 3,
    [&host](const Local<Value> arg0, string& err_msg) {
      if (!arg0->IsString()) return false;
      host = ToString(arg0);
      if (host.empty()) {
        err_msg = "hostname cannot be empty";
        return false;
      }
      return true;
    },
    [&port](const Local<Value> arg1, string& err_msg) {
      if (!arg1->IsUint32()) return false;
      port = ToUint32( arg1);
      if (0 == port) {
        err_msg = "port number cannot be zero";
        return false;
      }
      return true;
    },
    [&loc](const Local<Value> arg2, string& err_msg) {
      if (!arg2->IsString()) return false;
      loc = ToString(arg2);
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

void Subscriber::SubscriberConfig::streamNotifier(const FunctionCallbackInfo<Value>& args) {
  auto host = ""s;
  auto port = 0u;
  auto tag = ""s;
  if (!CheckArgs("streamNotifier", args, 5, 5,
    [&host](const Local<Value> arg0, string& err_msg) {
      if (!arg0->IsString()) return false;
      host = ToString(arg0);
      if (host.empty()) {
        err_msg = "hostname cannot be empty";
        return false;
      }
      return true;
    },
    [&port](const Local<Value> arg1, string& err_msg) {
      if (!arg1->IsUint32()) return false;
      port = ToUint32(arg1);
      if (0 == port) {
        err_msg = "port number cannot be zero";
        return false;
      }
      return true;
    },
    [&tag](const Local<Value> arg2, string& err_msg) {
      if (!arg2->IsString()) return false;
      tag = ToString(arg2);
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
  self->config_.use_tls_for_notifier = ToBool(args[3]);
  self->config_.no_notifier_cert_check = !ToBool(args[4]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::duration(const FunctionCallbackInfo<Value>& args) {
  at::Duration dur = 0s;
  if (!CheckArgs("duration", args, 1, 1,
    [&dur](const Local<Value> arg0, string& err_msg) {
      if (!arg0->IsString()) return false;
      try {
        dur = at::eastwood::DurationFromString(ToString(arg0));
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

void Subscriber::SubscriberConfig::userId(const FunctionCallbackInfo<Value>& args) {
  auto uid = ""s;
  if (!CheckArgs("userId", args, 1, 1,
    [&uid](const Local<Value> arg0, string& err_msg) {
      if (!arg0->IsString()) return false;
      uid = ToString(arg0);
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

void Subscriber::SubscriberConfig::streamUrl(const FunctionCallbackInfo<Value>& args) {
  auto url = ""s;
  if (!CheckArgs("streamUrl", args, 1, 1,
    [&url](const Local<Value> arg0, string& err_msg) {
      if (!arg0->IsString()) return false;
      url = ToString(arg0);
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

void Subscriber::SubscriberConfig::certCheck(const FunctionCallbackInfo<Value>& args) {
  if (!CheckArgs("certCheck", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsBoolean(); })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->config_.no_cert_check = !ToBool(args[0]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::authSecret(const FunctionCallbackInfo<Value>& args) {

  if (!CheckArgs("authSecret", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsString(); })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->config_.auth_secret = ToString(args[0]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::printFrameInfo(const FunctionCallbackInfo<Value>& args) {

  if (!CheckArgs("printFrameInfo", args, 1, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsBoolean(); })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->config_.print_frame_info = ToBool(args[0]);
  args.GetReturnValue().Set(args.Holder());
}

namespace {

bool CheckSinkArg(Isolate* isolate, Local<Context> context, const string& type, Local<Value> arg,
                  int32_t sinkNoneEnum, int32_t sinkFileEnum,
                  int32_t& sink_type, string& filename,
                  string& err_msg) {
  if (!arg->IsObject()) return false;
  auto sink_obj = arg->ToObject(context).ToLocalChecked();
  auto maybe_sink = sink_obj->Get(context, ToLocalString("sink"));
  if (maybe_sink.IsEmpty()) {
    err_msg = "Need " + type + " sink type";
    return false;
  }
  auto sink_val = maybe_sink.ToLocalChecked();
  if (!sink_val->IsInt32()) {
    err_msg = "Incorrect " + type + " sink type " + Inspect(sink_val);
    return false;
  }
  auto sink = ToInt32(sink_val);
  if (sinkNoneEnum == sink) {
    sink_type = sink;
    return true;
  }
  if (sinkFileEnum != sink) {
    err_msg = "Incorrect " + type + " sink type " + to_string(sink);
    return false;
  }
  auto maybe_file = sink_obj->Get(context, ToLocalString("filename"));
  if (maybe_file.IsEmpty()) {
    err_msg = "Need " + type + " sink filename";
    return false;
  }
  auto file = maybe_file.ToLocalChecked();
  if (!file->IsString()) {
    err_msg = "Wrong " + type + " sink filename " + Inspect(file);
    return false;
  }
  sink_type = sink;
  filename = ToString(file);
  return true;
}


}  // anonymous namespace

void Subscriber::SubscriberConfig::sink(const FunctionCallbackInfo<Value>& args) {
  auto isolate = args.GetIsolate();
  auto context = isolate->GetCurrentContext();
  auto audio_sink = static_cast<int32_t>(EastWood::AudioSink_None);
  auto audio_filename = ""s;
  auto video_sink = static_cast<int32_t>(EastWood::VideoSink_None);
  auto video_filename = ""s;

  if (!CheckArgs("sink", args, 2, 2,
        [isolate, context, &audio_sink, &audio_filename](const Local<Value> arg0, string& err_msg) {
          return CheckSinkArg(isolate, context, "audio", arg0,
                              static_cast<int32_t>(EastWood::AudioSink_None),
                              static_cast<int32_t>(EastWood::AudioSink_File),
                              audio_sink, audio_filename,
                              err_msg);
        },
        [isolate, context, &video_sink, &video_filename](const Local<Value> arg1, string& err_msg) {
          return CheckSinkArg(isolate, context, "video", arg1,
                              static_cast<int32_t>(EastWood::VideoSink_None),
                              static_cast<int32_t>(EastWood::VideoSink_File),
                              video_sink, video_filename,
                              err_msg);
        })) return;

  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  self->audio_sink_ = static_cast<EastWood::SinkType>(audio_sink);
  self->audio_sink_filename_ = audio_filename;
  self->video_sink_ = static_cast<EastWood::SinkType>(video_sink);
  self->video_sink_filename_ = video_filename;
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::ffmpegSink(const FunctionCallbackInfo<Value>& args) {
  auto output = ""s;
  if (!CheckArgs("ffmpegSink", args, 2, 2,
        [&output](const Local<Value> arg0, string& err_msg) {
          if (!arg0->IsString()) return false;
          output = ToString(arg0);
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
  self->ffmpeg_param_ = ToString(args[1]);
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::SubscriberConfig::subscriptionErrorRetry(const FunctionCallbackInfo<Value>& args) {
  auto max_retries = 0u;
  auto progression = 0.0;
  if (!CheckArgs("subscriptionErrorRetry", args, 3, 3,
    [&max_retries](const Local<Value> arg0, string& err_msg) {
      if (!arg0->IsUint32()) return false;
      max_retries = ToUint32(arg0);
      return true;
    },
    [](const Local<Value> arg1, string& err_msg) { return arg1->IsUint32(); },
    [&max_retries, &progression](const Local<Value> arg2, string& err_msg) {
      if (!arg2->IsNumber()) return false;
      if (0 < max_retries) {
        progression = ToDouble(arg2);
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
  self->config_.err_retry_delay_init_ms = ToUint32(args[1]);;
  self->config_.err_retry_delay_progression = progression;
  args.GetReturnValue().Set(args.Holder());
}

void Subscriber::on(const FunctionCallbackInfo<Value>& args) {
  if (!CheckArgs("on", args, 2, 2,
    [](const Local<Value> arg0, string& err_msg) {
      if (!arg0->IsString()) return false;
      auto event = ToString(arg0);
      return ("finish" == event);
    },
    [](const Local<Value> arg1, string& err_msg) { return arg1->IsFunction(); })) return;

  Subscriber* self = Unwrap<Subscriber>(args.Holder());
  assert(self);

  self->finish_event_.AddListener(Local<Function>::Cast(args[1]));
}

void Subscriber::start(const FunctionCallbackInfo<Value>& args) {
  if (!CheckArgs("start", args, 0, 0)) return;
  Subscriber* self = Unwrap<Subscriber>(args.Holder());
  assert(self);

  AT_LOG_INFO(self->log_, "Starting");

  SubscriberConfig* config = Unwrap<SubscriberConfig>(self->config_.Get(args.GetIsolate()));
  assert(config);
  if (!config->VerifyConfigIntegrity(args)) {
    // thrown
    return;
  }

  // starts event emission
  self->finish_event_.Start();

  if (!self->CreateSinks(*config)) {
    AT_LOG_ERROR(self->log_, "Failed to create sinks");
    return;
  }

  // Lazy init of facade
  if (!self->facade_) {
    self->facade_ = SubscriberFacade::New(EastWood::event_loop, move(config->config_));
  }

  self->facade_->on_finished([self]() {
      self->NotifyFinish();
  });
  self->facade_->Start();
  AT_LOG_INFO(self->log_, "Started");
}

bool Subscriber::CreateSinks(SubscriberConfig& config) {
  // a/v sink integrity has been checked already, so just checking one of them is sufficient here.
  if (!config.ffmpeg_output_.empty()) {
    return CreateFFMpegSinks(config);
  } else {
    return CreateRegularSinks(config);
  }
}

bool Subscriber::CreateRegularSinks(SubscriberConfig& config) {
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
  return true;
}

static pair<string, string> SplitKeyValue(const string& token, char separator) {
  auto pos = token.find(separator);
  if (pos < token.size()) {
    return make_pair(token.substr(0, pos), token.substr(pos+1));
  } else {
    return make_pair(token, ""s);
  }
}

bool Subscriber::CreateFFMpegSinks(SubscriberConfig& config) {
  using at::eastwood::FFmpegStreamSinkFactory;
  FFmpegStreamSinkFactory::OptionMap opts;
  auto params = at::eastwood::StringTokenizer::Tokenize(config.ffmpeg_param_, ' ');
  for (const auto& param : params) {
    auto key_val = SplitKeyValue(param, '=');  // key-value separator
    opts[key_val.first] = key_val.second;
  }
  auto factory = FFmpegStreamSinkFactory(config.ffmpeg_output_, opts);

  bool result = true;

  // register error handler
  factory.RegisterRoomIdleTimeoutHandler([this]() {
    NotifyFinish(kErrorIdleTimeout);
  });
  factory.RegisterStreamOutputFailureHandler([this, &result]() {
    result = false;  // it can fail in CreateSinks. this is to capture it.
    NotifyFinish(kErrorOutputFailure);
  });

  auto a_v_sinks = factory.CreateSinks(at::eastwood::AudioSinkConfig(), at::eastwood::VideoSinkConfig());
  config.config_.audio_sink = move(a_v_sinks.first);
  config.config_.video_sink = move(a_v_sinks.second);
  return result;
}

void Subscriber::NotifyFinish(const string& err) {
  AT_LOG_INFO(log_, "Notifying finish: " << err);
  if (err.empty()) {
    finish_event_.Emit(nullptr);
  } else {
    finish_event_.Emit(V8Error(err));
  }
}

void Subscriber::stop(const FunctionCallbackInfo<Value>& args) {
  v8::HandleScope scope(args.GetIsolate());

  if (!CheckArgs("stop", args, 0, 1,
    [](const Local<Value> arg0, string& err_msg) { return arg0->IsFunction(); }
  )) return;
  Subscriber* self = Unwrap<Subscriber>(args.Holder());
  assert(self);

  if (1 == args.Length()) {
    self->stop_callback_.SetCallbackFn(Local<Function>::Cast(args[0]));
  } else {
    auto isolate = args.GetIsolate();
    self->stop_callback_.SetCallbackFn(
                               Function::New(isolate->GetCurrentContext(),
                                   [](const FunctionCallbackInfo<Value>& args){}).ToLocalChecked());
  }

  self->StopFacade([self](exception_ptr ex, bool result) {
    if (ex) {
      AT_LOG_ERROR(self->log_, "Stop callback with exception: " << ex);
      self->stop_callback_.Call(false);
      return;
    }
    self->stop_callback_.Call(result);
  });
}

void Subscriber::StopFacade(std::function<void(exception_ptr, bool)> callback) {
  AT_LOG_INFO(log_, "Stopping");

  finish_event_.Stop();

  if (!facade_) {
    // Stopped before Start... pretending 'stopped'
    if (callback) callback(nullptr, true);
    return;
  }

  if (!callback) callback = [](exception_ptr ex, bool s){};
  facade_->Stop()->on_result(callback);
}

void Subscriber::SubscriberConfig::verify(const FunctionCallbackInfo<Value>& args) {
  if (!CheckArgs("verify", args, 0, 0)) return;
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
   && ((video_sink_ == EastWood::Sink_Undefined) && (audio_sink_ == EastWood::Sink_Undefined))) {
    err += "Need sink\n";
  }
  if (!ffmpeg_output_.empty()
   && ((video_sink_ != EastWood::Sink_Undefined) || (audio_sink_ != EastWood::Sink_Undefined))) {
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
    ThrowException(args, Exception::Error, err);
    return false;
  } else {
    AT_LOG_INFO(log_, "Configuration verified OK");
    return true;
  }
}

void Subscriber::SubscriberConfig::toObject(const v8::FunctionCallbackInfo<v8::Value>& args) {
  SubscriberConfig* self = Unwrap<SubscriberConfig>(args.Holder());
  assert(self);
  args.GetReturnValue().Set(self->ToObjectImpl());
}

Local<Object> Subscriber::SubscriberConfig::ToObjectImpl() const {
  auto isolate = Isolate::GetCurrent();
  auto context = isolate->GetCurrentContext();
  auto obj = Object::New(isolate);
  auto bixby = Object::New(isolate);
  obj->Set(context, ToLocalString("bixby"), bixby).FromJust();
  bixby->Set(context, ToLocalString("host"),
                      ToLocalString(config_.bixby_endpoint.host())).FromJust();
  bixby->Set(context, ToLocalString("port"),
                      ToLocalInteger(config_.bixby_endpoint.port())).FromJust();
  auto allocator = Object::New(isolate);
  obj->Set(context, ToLocalString("allocator"), allocator).FromJust();
  allocator->Set(context, ToLocalString("host"),
                      ToLocalString(config_.bixby_allocator_endpoint.host())).FromJust();
  allocator->Set(context, ToLocalString("port"),
                      ToLocalInteger(config_.bixby_allocator_endpoint.port())).FromJust();
  allocator->Set(context, ToLocalString("loc"),
                      ToLocalString(config_.alloc_location)).FromJust();
  auto notifier = Object::New(isolate);
  obj->Set(context, ToLocalString("notifier"), notifier).FromJust();
  notifier->Set(context, ToLocalString("host"),
                      ToLocalString(config_.notifier_endpoint.host())).FromJust();
  notifier->Set(context, ToLocalString("port"),
                      ToLocalInteger(config_.notifier_endpoint.port())).FromJust();
  notifier->Set(context, ToLocalString("tag"),
                      ToLocalString(config_.tag)).FromJust();
  notifier->Set(context, ToLocalString("tls"),
                      ToLocalBoolean(config_.use_tls_for_notifier)).FromJust();
  notifier->Set(context, ToLocalString("cert"),
                      ToLocalBoolean(!config_.no_notifier_cert_check)).FromJust();
  obj->Set(context, ToLocalString("duration_ms"),
                      ToLocalInteger(chrono::duration_cast<chrono::milliseconds>(config_.duration).count())).FromJust();
  obj->Set(context, ToLocalString("userId"),
                      ToLocalString(config_.user_id)).FromJust();
  obj->Set(context, ToLocalString("streamURL"),
                      ToLocalString(config_.stream_url)).FromJust();
  obj->Set(context, ToLocalString("cert"),
                      ToLocalBoolean(!config_.no_cert_check)).FromJust();
  obj->Set(context, ToLocalString("secret"),
                      ToLocalString(config_.auth_secret)).FromJust();
  obj->Set(context, ToLocalString("frameInfo"),
                      ToLocalBoolean(config_.print_frame_info)).FromJust();
  if (!ffmpeg_output_.empty()) {
    auto ffmpeg = Object::New(isolate);
    obj->Set(context, ToLocalString("ffmpeg"), ffmpeg).FromJust();
    ffmpeg->Set(context, ToLocalString("output"),
                         ToLocalString(ffmpeg_output_)).FromJust();
    ffmpeg->Set(context, ToLocalString("params"),
                         ToLocalString(ffmpeg_param_)).FromJust();
  } else {
    auto audio = Object::New(isolate);
    obj->Set(context, ToLocalString("audio"), audio).FromJust();
    audio->Set(context, ToLocalString("sink"),
                        ToLocalString(EastWood::SinkString(audio_sink_))).FromJust();
    audio->Set(context, ToLocalString("filename"),
                        ToLocalString(audio_sink_filename_)).FromJust();
    auto video = Object::New(isolate);
    obj->Set(context, ToLocalString("video"), video).FromJust();
    video->Set(context, ToLocalString("sink"),
                        ToLocalString(EastWood::SinkString(video_sink_))).FromJust();
    video->Set(context, ToLocalString("filename"),
                        ToLocalString(video_sink_filename_)).FromJust();
  }
  auto retry = Object::New(isolate);
  obj->Set(context, ToLocalString("retry"), retry).FromJust();
  retry->Set(context, ToLocalString("max"),
                      ToLocalInteger(config_.err_max_retries)).FromJust();
  retry->Set(context, ToLocalString("initDelay_ms"),
                      ToLocalInteger(config_.err_retry_delay_init_ms)).FromJust();
  retry->Set(context, ToLocalString("progression"),
                      ToLocalNumber(config_.err_retry_delay_progression)).FromJust();
  return obj;
}


Subscriber::SubscriberConfig::SubscriberConfig() 
  : log_(at::log::keywords::channel = "addon.SubscriberConfig") {
}

void Subscriber::SubscriberConfig::Init(Local<Object> exports) {
  InitClass(exports, "SubscriberConfig", New, constructor,
    AT_ADDON_PROTOTYPE_METHOD(bixby),
    AT_ADDON_PROTOTYPE_METHOD(bixbyAllocator),
    AT_ADDON_PROTOTYPE_METHOD(streamNotifier),
    AT_ADDON_PROTOTYPE_METHOD(duration),
    AT_ADDON_PROTOTYPE_METHOD(userId),
    AT_ADDON_PROTOTYPE_METHOD(streamUrl),
    AT_ADDON_PROTOTYPE_METHOD(certCheck),
    AT_ADDON_PROTOTYPE_METHOD(authSecret),
    AT_ADDON_PROTOTYPE_METHOD(printFrameInfo),
    AT_ADDON_PROTOTYPE_METHOD(sink),
    AT_ADDON_PROTOTYPE_METHOD(ffmpegSink),
    AT_ADDON_PROTOTYPE_METHOD(subscriptionErrorRetry),
    AT_ADDON_PROTOTYPE_METHOD(verify),
    AT_ADDON_PROTOTYPE_METHOD(toObject)
  );
}

Local<Object> Subscriber::SubscriberConfig::NewInstance(const FunctionCallbackInfo<Value>& args) {
  return NewV8Instance(constructor, args);
}

void Subscriber::SubscriberConfig::New(const FunctionCallbackInfo<Value>& args) {
  NewCppInstance(args, new SubscriberConfig());
}



void Subscriber::Init(Local<Object> exports) {
  InitClass(exports, "Subscriber", New, constructor,
    AT_ADDON_PROTOTYPE_METHOD(configuration),
    AT_ADDON_PROTOTYPE_METHOD(on),
    AT_ADDON_PROTOTYPE_METHOD(start),
    AT_ADDON_PROTOTYPE_METHOD(stop)
  );

  SubscriberConfig::Init(exports);
}

Local<Object> Subscriber::NewInstance(const FunctionCallbackInfo<Value>& args) {
  return NewV8Instance(constructor, args);
}

void Subscriber::New(const FunctionCallbackInfo<Value>& args) {
  NewCppInstance(args, new Subscriber(args));
}

}  // namespace ew
