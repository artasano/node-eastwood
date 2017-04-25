/// @copyright © 2017 Airtime Media.  All rights reserved.

#include <algorithm>
#include <utility>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "eastwood.h"
#include "subscriber.h"
#include "util/addon_util.h"

namespace ew {

using v8::Function;
using v8::FunctionCallbackInfo;
using v8::Persistent;

using namespace std;
using namespace string_literals;
using namespace chrono_literals;

using namespace at::node_addon;

Persistent<Function> EastWood::constructor;
at::Ptr<at::EventLoop> EastWood::event_loop;


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
  if (!event_loop) {
    event_loop = at::EventLoop::New(max<uint32_t>(1, at::EventLoopImpl::GetDefaultNumThreads() - 2));
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
    case AudioSink_Undefined:
      return "undefined";
    default:
      return "invalid(" + to_string(static_cast<int>(sink)) + ")";
  }
}

string EastWood::VideoSinkString(VideoSinkType sink) {
  switch (sink) {
    case VideoSink_None:
      return "none";
    case VideoSink_File:
      return "file";
    case VideoSink_Undefined:
      return "undefined";
    default:
      return "invalid(" + to_string(static_cast<int>(sink)) + ")";
  }
}

void EastWood::Init(Local<Object> exports) {
  InitClass(exports, "EastWood", New, constructor,
    AT_ADDON_PROTOTYPE_METHOD(createSubscriber),

    AT_ADDON_CLASS_CONSTANT(LogLevel_Fatal),
    AT_ADDON_CLASS_CONSTANT(LogLevel_Error),
    AT_ADDON_CLASS_CONSTANT(LogLevel_Warning),
    AT_ADDON_CLASS_CONSTANT(LogLevel_Info),
    AT_ADDON_CLASS_CONSTANT(LogLevel_Debug),

    AT_ADDON_CLASS_CONSTANT(AudioSink_None),
    AT_ADDON_CLASS_CONSTANT(AudioSink_File),
    AT_ADDON_CLASS_CONSTANT(VideoSink_None),
    AT_ADDON_CLASS_CONSTANT(VideoSink_File)
  );

  Subscriber::Init(exports);
}

void EastWood::New(const FunctionCallbackInfo<Value>& args) {
  auto log_level = LogLevel_Info;
  if (!CheckArgs("EastWood", args, 3, 4,
      [&log_level](Local<Value> arg0, string& err_msg) {
        if (!arg0->IsNumber()) return false;
        log_level = static_cast<LogLevel>(ToInt32(arg0));
        if (log_level < LogLevel_Fatal || LogLevel_Debug < log_level) {
          err_msg = "Incorrect log level value " + to_string(log_level);
          return false;
        }
        return true;
      },
      [](Local<Value> arg1, string& err_msg) { return arg1->IsBoolean(); },
      [](Local<Value> arg2, string& err_msg) { return arg2->IsBoolean(); },
      [](Local<Value> arg3, string& err_msg) { return arg3->IsString(); })) return;

  auto log_to_console = ToBool(args[1]);
  auto log_to_syslog = ToBool(args[2]);
  auto log_props_file = ((3 < args.Length()) ? ToString(args[3]) : ""s);
  NewCppInstance<EastWood>(args, new EastWood(log_level, log_to_console, log_to_syslog, log_props_file));
}

void EastWood::createSubscriber(const FunctionCallbackInfo<Value>& args) {
  if (!CheckArgs("createSubscriber", args, 0, 0)) return;

  args.GetReturnValue().Set(Subscriber::NewInstance(args));
}

}  // namespace ew
