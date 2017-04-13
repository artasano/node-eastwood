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


namespace ew {

class Subscriber : public node::ObjectWrap {
 public:
  /// Chainable configuration builder
  class SubscriberConfig : public node::ObjectWrap {
   public:
    /// @internal constructed only by Subscriber
    SubscriberConfig();

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

    enum AudioSinkType { AudioSinkNone = 0, AudioSinkFile = 1, AudioSinkFFMpeg = 2 };
    /**
     * Sets audio sink (optional. default is null-sink)
     * C++ Equivalence:
     * SubscriberConfig& AudioSink(AudioSinkType sink, const string& param = "");
     * @param param: filename for file sink, ffmpeg parameters for ffmpeg sink
     */
    static void AudioSink(const v8::FunctionCallbackInfo<v8::Value>& args);

    enum VideoSinkType { VideoSinkNone = 0, VideoSinkFile = 1, VideoSinkFFMpeg = 2 };
    /**
     * Sets vodeo sink (optional. default is null-sink)
     * C++ Equivalence:
     * SubscriberConfig& VideoSink(VideoSinkType sink, const string& param = "");
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
    AudioSinkType audio_sink_ = AudioSinkNone;
    std::string audio_sink_param_;
    VideoSinkType video_sink_ = VideoSinkNone;
    std::string video_sink_param_;
    uint32_t err_max_retries_ = 0;
    uint32_t err_retry_init_delay_ms_ = 0;
    float err_retry_delay_progression_ = 1.0;

    void VerifyConfig(v8::Isolate* isolate) const;
    void VerifyBixbyConfig(v8::Isolate* isolate) const;
    void VerifyAllocatorConfig(v8::Isolate* isolate) const;
    void VerifyNotifierConfig(v8::Isolate* isolate) const;
    void VerifyDurationConfig(v8::Isolate* isolate) const;
    void VerifyUserIdConfig(v8::Isolate* isolate) const;
    void VerifyStreamUrlConfig(v8::Isolate* isolate) const;
    void VerifyTagConfig(v8::Isolate* isolate) const;
    void VerifyAudioSinkConfig(v8::Isolate* isolate) const;
    void VerifyVideoSinkConfig(v8::Isolate* isolate) const;
    void VerifyErrorRetryConfig(v8::Isolate* isolate) const;
    void VerifyConfigCombinations(v8::Isolate* isolate) const;
    static std::string AudioSinkString(AudioSinkType sink);
    static std::string VideoSinkString(VideoSinkType sink);

    static v8::Local<v8::Object> NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);
    /// @internal Used by V8 framework
    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
    /// @internal Used by V8 framework
    static v8::Persistent<v8::Function> constructor;
    /// @internal Used by V8 framework
    static void Init(v8::Local<v8::Object> exports);

    friend class Subscriber;
    friend std::ostream& operator<< (std::ostream& ost, const SubscriberConfig& config);
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

  /**
   * Dumps config
   * C++ Equivalent string DumpConfig() const;
   */
  static void DumpConfig(const v8::FunctionCallbackInfo<v8::Value>& args);
  
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

  static v8::Local<v8::Object> NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);

  friend class EastWood;

  mutable at::Logger log_;
  static at::Ptr<at::EventLoop> event_loop_;
  v8::Local<v8::Object> config_;
  std::vector<v8::Local<v8::Function>> started_event_listeners_;
  std::vector<v8::Local<v8::Function>> ended_event_listeners_;
  std::vector<v8::Local<v8::Function>> error_event_listeners_;
};

class EastWood : public node::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);

 private:
  EastWood();
  ~EastWood();

  /**
   * @param init value
   */
  static void CreateSubscriber(const v8::FunctionCallbackInfo<v8::Value>& args);

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;
};

}  // namespace ew

#endif  // EASTWOOD_H_

