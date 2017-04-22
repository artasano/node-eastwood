/// @copyright © 2017 Airtime Media.  All rights reserved.

#ifndef EASTWOOD_H_
#define EASTWOOD_H_

#include <node.h>
#include <node_object_wrap.h>

#include <iostream>
#include <string>
#include <vector>
#include "mediacore/defs.h"
#include "tecate/defs.h"
#include "mediacore/async/eventloop.h"
#include "mediacore/base/logging.h"
#include "addon_util.h"


namespace ew {

class EastWood : public node::ObjectWrap {
 public:

// TODO(Art): combine a/v sink type
  enum AudioSinkType { AudioSink_None = 0, AudioSink_File = 1, AudioSink_Undefined = -1 };
  enum VideoSinkType { VideoSink_None = 0, VideoSink_File = 1, VideoSink_Undefined = -1 };
  enum LogLevel { LogLevel_Fatal = 0, LogLevel_Error = 1, LogLevel_Warning = 2, LogLevel_Info = 3, LogLevel_Debug = 4 };

  static std::string AudioSinkString(AudioSinkType sink);
  static std::string VideoSinkString(VideoSinkType sink);

  static void Init(v8::Local<v8::Object> exports);

  static at::Ptr<at::EventLoop> event_loop;

 private:
  EastWood(LogLevel level,
           bool log_to_console, bool log_to_syslog, const std::string& log_props_file = "");
  ~EastWood();

  /**
   * Creates a Subscriber instance.
   * C++ Equivalence:
   *  Subscriber createSubscriber();
   * @param init value
   */
  static void CreateSubscriber(const v8::FunctionCallbackInfo<v8::Value>& args);

  mutable at::Logger log_;

  /// @internal called by V8 framewodk
  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;

  AT_ADDON_CLASS;
};

}  // namespace ew

#endif  // EASTWOOD_H_

