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
#include "addon_util/addon_util.h"


namespace ew {

class Subscriber : public node::ObjectWrap {
 public:
  /// Chainable configuration builder
  class SubscriberConfig : public node::ObjectWrap {
   private:
    // private because these are indirectly called via V8 framework.

    /**
     * Sets bixby endpoint - mutually exclusive to bixby allocator endpoint.
     * Signature:
     * SubscriberConfig bixby(String host, Number port);
     * @return self
     * @param host: bixby host
     * @param port: bixby port (must be greater than zero)
     */
    static void bixby(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets bixby allocator endpoint - mutually exclusive to bixby endpoint.
     * Signature:
     *   SubscriberConfig bixbyAllocator(String host, Number port, String locationUrlOrIp);
     * @return self
     * @param host: bixby allocator host
     * @param port: bixby allocator port (must be greater than zero)
     * @param locationUrlOrIp: URL of IP address specifying location of allocatee
     */
    static void bixbyAllocator(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets stream notifier endpoint - mutually exclusive to streamUrl.
     * Signature:
     *   SubscriberConfig streamNotifier(
     *         String host, Number port, String tag,
     *         Boolean useTls, Boolean enableCertCheck);
     * @return self
     * @param host: stream notifier host
     * @param port: stream notifier port (must be greater than zero)
     * @param tag: tag
     * @param useTls: whether to use TLS
     * @param enableCertCheck: whether to check server cert
     */
    static void streamNotifier(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets subscribing duration 'hh:mm:ss' (optional. default is 'infinite')
     * Signature:
     *   SubscriberConfig duration(String duration);
     * @return self
     * @param duration: either "hh:mm:ss" or "infinite"
     */
    static void duration(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets user id (mandatory)
     * Signature:
     *   SubscriberConfig userId(String uid);
     * @return self
     * @param uid: user id
     */
    static void userId(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets stream URL - mutually exclusive to stream-notifier and tag
     * Signature:
     *   SubscriberConfig streamUrl(String streamUrl);
     * @return self
     * @param StreamUrl: stream URL
     */
    static void streamUrl(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Disables cert check (optional. default is enabled)
     * Signature:
     *   SubscriberConfig certCheck(Boolean check);
     * @return self
     * @param check: whether to check server cert of Bixby websocket
     */
    static void certCheck(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets auth secret (optional. no auth token is sent if not given)
     * Signature:
     *   SubscriberConfig authSecret(String secret);
     * @return self
     * @param secret: auth secret
     */
    static void authSecret(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Enables extra debug log for each video frame (optional. default is off)
     * Signature:
     *   SubscriberConfig printFrameInfo(Boolean print);
     * @return self
     * @param print: whether to debug-log every video frame received
     */
    static void printFrameInfo(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets regular audio/video sink (optional. default is null-sink)
     * Use FFMpegSink() to set up FFMpeg sinks. (these are mutually exclusive)
     * Signature:
     *   SubscriberConfig sink(Object audio_sink, Object video_sink);
     * @return self
     * @param audio_sink: { sink: EastWood::SinkType, filename: <filename> }
     * @param video_sink: { sink: EastWood::SinkType, filename: <filename> }
     */
    static void sink(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets FFMpeg audio/video sink (optional. default is regular null-sink)
     * Use sink() to set up regular sinks. (these are mutually exclusive)
     * Signature:
     *   SubscriberConfig ffmpegSink(String output, String param);
     * @return self
     * @param output: output destination (filename or rtmp-URL)
     * @param param: ffmpeg parameters. see libew-ffmpeg
     */
    static void ffmpegSink(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Sets subscription error retry. (optional. default is no-retry)
     * Signature:
     *   SubscriberConfig subscriptionErrorRetry(
     *            uint32_t maxRetries, uint32_t initialDelayMS,
     *            float delayProgressionFactor);
     * @return self
     * @param maxRetries: zero disables retry.
     * @param initialDelayMS: initial retry delay in milli-sec
     * @param delayProgressionFactor: must be 1.0 or above.
     *        delay increases exponentially with this factor.
     */
    static void subscriptionErrorRetry(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Verifies the given config params. Will be implicitly called by Subscriber::start()
     * Signature:
     *  void verify() const;
     * @throw JS Exception if any problem was found.
     */
    static void verify(const v8::FunctionCallbackInfo<v8::Value>& args);

    /**
     * Returns an JavaScript Object that contains a copy of current configuration.
     * Signature:
     *  Object toObject() const;
     * @return Object containing configuration
     */
    static void toObject(const v8::FunctionCallbackInfo<v8::Value>& args);

    /// @internal constructed only (indirectly) by Subscriber
    SubscriberConfig();

    at::eastwood::SubscriberConfig config_;
    EastWood::SinkType audio_sink_ = EastWood::Sink_Undefined;
    std::string audio_sink_filename_;
    EastWood::SinkType video_sink_ = EastWood::Sink_Undefined;
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
   * Signature:
   *  SubscriberConfig configuration();
   * @return config object to set and view
   */
  static void configuration(const v8::FunctionCallbackInfo<v8::Value>& args);

  /**
   * Registers event listener.
   * Signature:
   *  void on(String name, v8::Function callback)
   * @param name : 'finish' is the only event at this moment
   * @param callback : function(err)
   *
   * One callback will be given once started.
   * If FFMpeg sinks are used, @a err in "finish" event may contain string either 'idle timeout' or 'output failure'
   * For all sink types, other string in @a err maybe notified.
   */
  static void on(const v8::FunctionCallbackInfo<v8::Value>& args);

  static constexpr auto kErrorIdleTimeout = "idle timeout";
  static constexpr auto kErrorOutputFailure = "output failure";

  /**
   * Starts the subscription.
   * Signature:
   *  void start();
   * @throw exception if configuration is incomplete or incorrect.
   * @note It is important to call stop() in order to clean up resource.
   * Otherwise JS process might not terminate at the end.
   */
  static void start(const v8::FunctionCallbackInfo<v8::Value>& args);

  /**
   * Stops the subscription.
   * Signature:
   *  void stop(callback);
   * @param callback: function(bool)
   */
  static void stop(const v8::FunctionCallbackInfo<v8::Value>& args);

  /// @internal Used for V8 framework
  static void Init(v8::Local<v8::Object> exports);

  /// @internal only internally used
  explicit Subscriber(const v8::FunctionCallbackInfo<v8::Value>& args);

 private:
  ~Subscriber();
  bool CreateSinks(SubscriberConfig& config);
  bool CreateRegularSinks(SubscriberConfig& config);
  bool CreateFFMpegSinks(SubscriberConfig& config);
  void NotifyFinish(const string& err = "");
  void StopFacade(std::function<void(std::exception_ptr, bool)> callback = std::function<void(std::exception_ptr, bool)>());

  /// @internal Used by V8 framework
  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  /// @internal Used by V8 framework
  static v8::Persistent<v8::Function> constructor;

  /// @internal Used by EastWood
  static v8::Local<v8::Object> NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);
  friend class EastWood;

  mutable at::Logger log_;
  v8::Persistent<v8::Object> config_;
  at::node_addon::EventEmitter<at::node_addon::V8Exception> finish_event_;
  at::node_addon::CallbackInvoker<bool> stop_callback_;
  bool sink_output_failed_ = false;
  at::Ptr<at::eastwood::SubscriberFacade> facade_;

  AT_ADDON_CLASS;
};

}  // namespace ew

#endif  // SUBSCRIBER_H_
