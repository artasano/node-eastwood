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
#include "mediacore/base/endpoint.h"
#include "addon_util.h"


namespace ew {

class EastWood : public node::ObjectWrap {
 public:
  enum AudioSinkType { AudioSink_None = 0, AudioSink_File = 1, AudioSink_FFMpeg = 2 };
  enum VideoSinkType { VideoSink_None = 0, VideoSink_File = 1, VideoSink_FFMpeg = 2 };
  enum LogLevel { LogLevel_Fatal = 0, LogLevel_Error = 1, LogLevel_Warning = 2, LogLevel_Info = 3, LogLevel_Debug = 4 };

  static std::string AudioSinkString(AudioSinkType sink);
  static std::string VideoSinkString(VideoSinkType sink);

  static void Init(v8::Local<v8::Object> exports);

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
  static at::Ptr<at::EventLoop> event_loop_;

  /// @internal called by V8 framewodk
  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;

  AT_ADDON_CLASS;
};

class Subscriber : public node::ObjectWrap {
 public:
  /// Chainable configuration builder
  class SubscriberConfig : public node::ObjectWrap {
   private:
    // private because these are indirectly called via V8 framework.
  
    /**
     * Sets bixby endpoint - mutually exclusive to bixby allocator endpoint.
     * C++ Equivalence:
     * SubscriberConfig& Bixby(const string& host, uint32_t port);
     */
    static void Bixby(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets bixby allocator endpoint - mutually exclusive to bixby endpoint.
     * C++ Equivalence:
     * SubscriberConfig& BixbyAllocator(const string& host, uint32_t port, const string& locationUrlOrIp);
     */
    static void BixbyAllocator(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets stream notifier endpoint - mutually exclusive to streamUrl.
     * C++ Equivalence:
     * SubscriberConfig& StreamNotifier(
     *         const string& host, uint32_t port, const string& tag,
     *         bool useTls, bool enableCertCheck);
     */
    static void StreamNotifier(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets subscribing duration 'hh:mm:ss' (optional. default is 'infinite')
     * C++ Equivalence:
     * SubscriberConfig& Duration(const string& duration);
     */
    static void Duration(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets user id (optional. randomly generated if not given)
     * C++ Equivalence:
     * SubscriberConfig& UserId(const string& uid);
     */
    static void UserId(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets stream URL - mutually exclusive to stream-notifier and tag
     * C++ Equivalent:
     * SubscriberConfig& StreamUrl(const string& streamUrl);
     */
    static void StreamUrl(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Disables cert check (optional. default is enabled)
     * C++ Equivalence:
     * SubscriberConfig& CertCheck(bool check);
     */
    static void CertCheck(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets auth secret (optional. no auth token is sent if not given)
     * C++ Equivalence:
     * SubscriberConfig& AuthSecret(const string& secret);
     */
    static void AuthSecret(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Enables extra debug log for each video frame (optional. default is off)
     * C++ Equivalence:
     * SubscriberConfig& PrintFrameInfo(bool print);
     */
    static void PrintFrameInfo(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets audio sink (optional. default is null-sink)
     * C++ Equivalence:
     * SubscriberConfig& AudioSink(EastWood::AudioSinkType sink, const string& param = "");
     * @param param: filename for file sink, ffmpeg parameters for ffmpeg sink
     */
    static void AudioSink(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets vodeo sink (optional. default is null-sink)
     * C++ Equivalence:
     * SubscriberConfig& VideoSink(EastWood::VideoSinkType sink, const string& param = "");
     * @param param: filename for file sink, ffmpeg parameters for ffmpeg sink
     */
    static void VideoSink(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets subscription error retry. (optional. default is no-retry)
     * C++ Equivalence:
     *     SubscriberConfig& SubscriptionErrorRetry(
     *            uint32_t maxRetries, uint32_t initialDelayMS,
     *            float delayProgressionFactor);
     * @param maxRetries: zero disables retry.
     * @param initialDelayMS: initial retry delay in milli-sec
     * @param delayProgressionFactor: must be 1.0 or above.
     *        delay increases exponentially with this factor.
     */
    static void SubscriptionErrorRetry(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Verifies the given config params. Will be implicitly called by Subscriber::start()
     * C++ Equivalence:
     *  void Verify() const;
     * @throw JS Exception if any problem was found.
     */
    static void Verify(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Returns an JavaScript Object that contains a copy of current configuration.
     * C++ Equivalence:
     * void ToObject() const;
     */
    static void ToObject(const v8::FunctionCallbackInfo<v8::Value>& args);

    /// @internal constructed only (indirectly) by Subscriber
    SubscriberConfig();

    at::Endpoint bixby_endpoint_;
    at::Endpoint allocator_endpoint_;
    std::string alloc_location_;  // used with bixby_allocator
    at::Endpoint notifier_endpoint_;
    std::string tag_;
    bool notifier_use_tls_ = false;
    bool notifier_cert_check_ = false;
    at::Duration duration_ = at::Duration::max();
    std::string user_id_;
    at::StreamUrl stream_url_;
    bool cert_check_ = true;
    std::string auth_secret_;
    bool print_frame_info_ = false;
    EastWood::AudioSinkType audio_sink_ = EastWood::AudioSink_None;
    std::string audio_sink_param_;
    EastWood::VideoSinkType video_sink_ = EastWood::VideoSink_None;
    std::string video_sink_param_;
    uint32_t err_max_retries_ = 0;
    uint32_t err_retry_init_delay_ms_ = 0;
    float err_retry_delay_progression_ = 1.0;

    v8::Local<v8::Object> ToObjectImpl(v8::Isolate* isolate) const;
    void VerifyAllConfig(const v8::FunctionCallbackInfo<v8::Value>& args) const;
    void VerifyBixbyConfig(const v8::FunctionCallbackInfo<v8::Value>& args) const;
    void VerifyAllocatorConfig(const v8::FunctionCallbackInfo<v8::Value>& args) const;
    void VerifyNotifierConfig(const v8::FunctionCallbackInfo<v8::Value>& args) const;
    void VerifyDurationConfig(const v8::FunctionCallbackInfo<v8::Value>& args) const;
    void VerifyUserIdConfig(const v8::FunctionCallbackInfo<v8::Value>& args) const;
    void VerifyStreamUrlConfig(const v8::FunctionCallbackInfo<v8::Value>& args) const;
    void VerifyTagConfig(const v8::FunctionCallbackInfo<v8::Value>& args) const;
    void VerifyAudioSinkConfig(const v8::FunctionCallbackInfo<v8::Value>& args) const;
    void VerifyVideoSinkConfig(const v8::FunctionCallbackInfo<v8::Value>& args) const;
    void VerifyErrorRetryConfig(const v8::FunctionCallbackInfo<v8::Value>& args) const;
    void VerifyConfigCombinations(const v8::FunctionCallbackInfo<v8::Value>& args) const;

    static v8::Local<v8::Object> NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);
    /// @internal Used by V8 framework
    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
    /// @internal Used by V8 framework
    static v8::Persistent<v8::Function> constructor;
    /// @internal Used by V8 framework
    static void Init(v8::Local<v8::Object> exports);

    friend class Subscriber;
    AT_ADDON_CLASS;
  };

  /**
   * Starts configure. Ex) Configure().bixbyEndPoint(xxx).userId(yyy)...
   * C++ Equivalent:
   * SubscriberConfig& Configure();
   */
  static void Configure(const v8::FunctionCallbackInfo<v8::Value>& args);

  /**
   * Registers event listener.
   * C++ Equivalence:
   * void On(const string& name, v8::Function callback)
   * @param name : 'started', 'ended', 'error'
   */
  static void On(const v8::FunctionCallbackInfo<v8::Value>& args);


  /**
   * Starts the subscription.
   * C++ Equivalent:
   * void Start();
   * 'started' event will be notified.
   * @throw exception if configuration is incomplete or incorrect.
   */
  static void Start(const v8::FunctionCallbackInfo<v8::Value>& args);

  /**
   * Stops the subscription
   * C++ Equivalent:
   * void Stop();
   * 'ended' event will be notified.
   */
  static void Stop(const v8::FunctionCallbackInfo<v8::Value>& args);

  /// @internal Used by V8 framework
  static void Init(v8::Local<v8::Object> exports);

  /// @internal only internally used
  explicit Subscriber(const v8::FunctionCallbackInfo<v8::Value>& args);

 private:
  ~Subscriber();

  /// @internal Used by V8 framework
  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  /// @internal Used by V8 framework
  static v8::Persistent<v8::Function> constructor;

  /// @internal Used by EastWood
  static v8::Local<v8::Object> NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);
  friend class EastWood;

  mutable at::Logger log_;
  v8::Persistent<v8::Object> config_;
  std::vector<v8::Local<v8::Function>> started_event_listeners_;
  std::vector<v8::Local<v8::Function>> ended_event_listeners_;
  std::vector<v8::Local<v8::Function>> error_event_listeners_;

  AT_ADDON_CLASS;
};

}  // namespace ew

#endif  // EASTWOOD_H_

