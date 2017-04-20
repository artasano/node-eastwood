/// @copyright © 2017 Airtime Media.  All rights reserved.

#ifndef SUBSCRIBER_H_
#define SUBSCRIBER_H_

#include <node.h>
#include <node_object_wrap.h>

#include <iostream>
#include <string>
#include <vector>

#include "mediacore/defs.h"
#include "tecate/defs.h"
#include "mediacore/base/logging.h"
#include "mediacore/base/endpoint.h"
#include "eastwood/subscribe/subscriber_config.h"
#include "facade/subscriber_facade.h"

#include "eastwood.h"
#include "addon_util.h"


namespace ew {

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
     * Sets user id (mandatory)
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
     * Sets regular audio sink (optional. default is null-sink)
     * Use FFMpegSink() to set up FFMpeg sinks. (these are mutually exclusive)
     * C++ Equivalence:
     * SubscriberConfig& AudioSink(EastWood::AudioSinkType sink, const string& filename = "");
     * @param filename: filename for file sink. (these are mutually exclusive)
     */
    static void AudioSink(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets regular video sink (optional. default is null-sink)
     * Use FFMpegSink() to set up FFMpeg sinks. (these are mutually exclusive)
     * C++ Equivalence:
     * SubscriberConfig& VideoSink(EastWood::VideoSinkType sink, const string& filename = "");
     * @param filename: filename for file sink, output destination for ffmpeg sink
     * @param param: ffmpeg parameters for ffmpeg sink
     */
    static void VideoSink(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets FFMpeg audio/video sink (optional. default is regular null-sink)
     * Use AudioSink() and VideoSink to set up regular sinks. (these are mutually exclusive)
     * C++ Equivalence:
     * SubscriberConfig& FFmpegSink(const string& output, const string& param);
     * @param output: output destination (filename or rtmp-URL)
     * @param param: ffmpeg parameters. see libew-ffmpeg
     */
    static void FFMpegSink(const v8::FunctionCallbackInfo<v8::Value>& args);

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

    at::eastwood::SubscriberConfig config_;
    EastWood::AudioSinkType audio_sink_ = EastWood::AudioSink_Undefined;
    std::string audio_sink_filename_;
    EastWood::VideoSinkType video_sink_ = EastWood::VideoSink_Undefined;
    std::string video_sink_filename_;
    std::string ffmpeg_output_;
    std::string ffmpeg_param_;

    v8::Local<v8::Object> ToObjectImpl(v8::Isolate* isolate) const;
    bool VerifyConfigIntegrity(const v8::FunctionCallbackInfo<v8::Value>& args) const;

    static v8::Local<v8::Object> NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);
    /// @internal Used by V8 framework
    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
    /// @internal Used by V8 framework
    static v8::Persistent<v8::Function> constructor;
    /// @internal Used for V8 framework
    static void Init(v8::Local<v8::Object> exports);

    mutable at::Logger log_;

    friend class Subscriber;
    AT_ADDON_CLASS;
  };

 private:
  /**
   * Accesses to configuration.
   * The configuration is available only until Start() is called.
   * The configuration object setters can be chain-called. Ex) Configuration().bixbyEndPoint(xxx).userId(yyy)...
   * C++ Equivalent:
   * SubscriberConfig& Configuration();
   */
  static void Configuration(const v8::FunctionCallbackInfo<v8::Value>& args);

  /**
   * Registers event listener.
   * C++ Equivalence:
   * void On(const string& name, v8::Function callback)
   * @param name : 'error' is the only event at this mement.
   * @param callback : function(err, subscriber)
   *
   * Either ended or error callback will be given once started.
   * If FFMpeg sinks are used, @a err in "on error" callback may contain string either 'idle timeout' or 'output failure'
   * For all sink types, other string in @a err maybe notified.
   */
  static void On(const v8::FunctionCallbackInfo<v8::Value>& args);

  static constexpr auto kErrorIdleTimeout = "idle timeout";
  static constexpr auto kErrorOutputFailure = "output failure";

  /**
   * Starts the subscription.
   * C++ Equivalent:
   * void Start();
   * @throw exception if configuration is incomplete or incorrect.
   */
  static void Start(const v8::FunctionCallbackInfo<v8::Value>& args);

  /**
   * Stops the subscription
   * C++ Equivalent:
   * void Stop(function<void(bool)>);
   */
  static void Stop(const v8::FunctionCallbackInfo<v8::Value>& args);

  /// @internal Used for V8 framework
  static void Init(v8::Local<v8::Object> exports);

  /// @internal only internally used
  explicit Subscriber(const v8::FunctionCallbackInfo<v8::Value>& args);

 private:
  ~Subscriber();
  void CreateSinks(SubscriberConfig& config);
  void CreateRegularSinks(SubscriberConfig& config);
  void CreateFFMpegSinks(SubscriberConfig& config);
  void NotifyError(const string& err);

  /// @internal Used by V8 framework
  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  /// @internal Used by V8 framework
  static v8::Persistent<v8::Function> constructor;

  /// @internal Used by EastWood
  static v8::Local<v8::Object> NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);
  friend class EastWood;

  mutable at::Logger log_;
  v8::Persistent<v8::Object> config_;
  std::vector<at::node_addon::PersistentFunctionCopyable> error_event_listeners_;
  at::node_addon::PersistentFunctionCopyable stop_callback_;
  std::unique_ptr<at::eastwood::SubscriberFacade> facade_;
  at::node_addon::Notifier notifier_;

  AT_ADDON_CLASS;
};

}  // namespace ew

#endif  // SUBSCRIBER_H_
