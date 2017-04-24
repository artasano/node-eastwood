/// @copyright © 2017 Airtime Media.  All rights reserved.

#ifndef SUBSCRIBER_H_
#define SUBSCRIBER_H_

#include <node.h>
#include <node_object_wrap.h>

#include <string>
#include <vector>

#include "mediacore/defs.h"
#include "tecate/defs.h"
#include "mediacore/base/logging.h"
#include "mediacore/base/endpoint.h"
#include "eastwood/subscribe/subscriber_config.h"
#include "facade/subscriber_facade.h"

#include "eastwood.h"
#include "util/addon_util.h"


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
     * SubscriberConfig& bixby(const string& host, uint32_t port);
     */
    static void bixby(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets bixby allocator endpoint - mutually exclusive to bixby endpoint.
     * C++ Equivalence:
     * SubscriberConfig& bixbyAllocator(const string& host, uint32_t port, const string& locationUrlOrIp);
     */
    static void bixbyAllocator(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets stream notifier endpoint - mutually exclusive to streamUrl.
     * C++ Equivalence:
     * SubscriberConfig& streamNotifier(
     *         const string& host, uint32_t port, const string& tag,
     *         bool useTls, bool enableCertCheck);
     */
    static void streamNotifier(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets subscribing duration 'hh:mm:ss' (optional. default is 'infinite')
     * C++ Equivalence:
     * SubscriberConfig& duration(const string& duration);
     */
    static void duration(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets user id (mandatory)
     * C++ Equivalence:
     * SubscriberConfig& userId(const string& uid);
     */
    static void userId(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets stream URL - mutually exclusive to stream-notifier and tag
     * C++ Equivalent:
     * SubscriberConfig& streamUrl(const string& streamUrl);
     */
    static void streamUrl(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Disables cert check (optional. default is enabled)
     * C++ Equivalence:
     * SubscriberConfig& certCheck(bool check);
     */
    static void certCheck(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets auth secret (optional. no auth token is sent if not given)
     * C++ Equivalence:
     * SubscriberConfig& authSecret(const string& secret);
     */
    static void authSecret(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Enables extra debug log for each video frame (optional. default is off)
     * C++ Equivalence:
     * SubscriberConfig& printFrameInfo(bool print);
     */
    static void printFrameInfo(const v8::FunctionCallbackInfo<v8::Value>& args);

// TODO(Art): combine a/v sink
    /**
     * Sets regular audio sink (optional. default is null-sink)
     * Use FFMpegSink() to set up FFMpeg sinks. (these are mutually exclusive)
     * C++ Equivalence:
     * SubscriberConfig& audioSink(EastWood::AudioSinkType sink, const string& filename = "");
     * @param filename: filename for file sink. (these are mutually exclusive)
     */
    static void audioSink(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets regular video sink (optional. default is null-sink)
     * Use FFMpegSink() to set up FFMpeg sinks. (these are mutually exclusive)
     * C++ Equivalence:
     * SubscriberConfig& videoSink(EastWood::VideoSinkType sink, const string& filename = "");
     * @param filename: filename for file sink, output destination for ffmpeg sink
     * @param param: ffmpeg parameters for ffmpeg sink
     */
    static void videoSink(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets FFMpeg audio/video sink (optional. default is regular null-sink)
     * Use AudioSink() and VideoSink to set up regular sinks. (these are mutually exclusive)
     * C++ Equivalence:
     * SubscriberConfig& ffmpegSink(const string& output, const string& param);
     * @param output: output destination (filename or rtmp-URL)
     * @param param: ffmpeg parameters. see libew-ffmpeg
     */
    static void ffmpegSink(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets subscription error retry. (optional. default is no-retry)
     * C++ Equivalence:
     *     SubscriberConfig& subscriptionErrorRetry(
     *            uint32_t maxRetries, uint32_t initialDelayMS,
     *            float delayProgressionFactor);
     * @param maxRetries: zero disables retry.
     * @param initialDelayMS: initial retry delay in milli-sec
     * @param delayProgressionFactor: must be 1.0 or above.
     *        delay increases exponentially with this factor.
     */
    static void subscriptionErrorRetry(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Verifies the given config params. Will be implicitly called by Subscriber::start()
     * C++ Equivalence:
     *  void verify() const;
     * @throw JS Exception if any problem was found.
     */
    static void verify(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Returns an JavaScript Object that contains a copy of current configuration.
     * C++ Equivalence:
     * void toObject() const;
     */
    static void toObject(const v8::FunctionCallbackInfo<v8::Value>& args);

    /// @internal constructed only (indirectly) by Subscriber
    SubscriberConfig();

    at::eastwood::SubscriberConfig config_;
    EastWood::AudioSinkType audio_sink_ = EastWood::AudioSink_Undefined;
    std::string audio_sink_filename_;
    EastWood::VideoSinkType video_sink_ = EastWood::VideoSink_Undefined;
    std::string video_sink_filename_;
    std::string ffmpeg_output_;
    std::string ffmpeg_param_;

    v8::Local<v8::Object> ToObjectImpl() const;
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
   * SubscriberConfig& configuration();
   */
  static void configuration(const v8::FunctionCallbackInfo<v8::Value>& args);

  /**
   * Registers event listener.
   * C++ Equivalence:
   * void on(const string& name, v8::Function callback)
   * @param name : 'error' is the only event at this mement.
   * @param callback : function(err, subscriber)
   *
   * Either ended or error callback will be given once started.
   * If FFMpeg sinks are used, @a err in "on error" callback may contain string either 'idle timeout' or 'output failure'
   * For all sink types, other string in @a err maybe notified.
   */
  static void on(const v8::FunctionCallbackInfo<v8::Value>& args);

  static constexpr auto kErrorIdleTimeout = "idle timeout";
  static constexpr auto kErrorOutputFailure = "output failure";

  /**
   * Starts the subscription.
   * C++ Equivalent:
   * void start();
   * @throw exception if configuration is incomplete or incorrect.
   */
  static void start(const v8::FunctionCallbackInfo<v8::Value>& args);

  /**
   * Stops the subscription
   * C++ Equivalent:
   * void stop(function<void(bool)>);
   */
  static void stop(const v8::FunctionCallbackInfo<v8::Value>& args);

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
  at::node_addon::EventEmitter error_event_;
  at::node_addon::CallbackInvoker stop_callback_;
  std::unique_ptr<at::eastwood::SubscriberFacade> facade_;

  AT_ADDON_CLASS;
};

}  // namespace ew

#endif  // SUBSCRIBER_H_
