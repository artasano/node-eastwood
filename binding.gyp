{
  "targets": [
    {
     "target_name": "eastwood_addon",
     "sources": [
       "src/eastwood_addon.cc",
       "src/eastwood.cc",
       "src/subscriber.cc",
       "node_modules/node-media-utils/src/addon_util/addon_util.cc"
     ],

      'libraries': [
        # relative path from build/ directory
        '../../build/osx-x86_64-release/mediacore/mediacore/libmediacore.a',
        '../../build/osx-x86_64-release/carmel/carmel/libcarmel.a',
        '../../build/osx-x86_64-release/carmel/carmel/libbixbyproto.a',
        '../../build/osx-x86_64-release/carmel/carmel/libat-wireproto.a',
        '../../build/osx-x86_64-release/libtecate/tecate/libtecate.a',
        '../../build/osx-x86_64-release/libtecate/tecate/libstream_notificationproto.a',
        '../../build/osx-x86_64-release/libtecate/tecate/libtecate_legacy_pubsubproto.a',
        '../../build/osx-x86_64-release/libtecate/tecate/libstream_managementproto.a',
        '../../build/osx-x86_64-release/libtecate/tecate/libtecate_commonproto.a',
        '../../build/osx-x86_64-release/eastwood-core/facade/libew-facade.a',
        '../../build/osx-x86_64-release/eastwood-core/eastwood/libew-common.a',
        '../../build/osx-x86_64-release/eastwood-core/eastwood/libew-source.a',
        '../../build/osx-x86_64-release/eastwood-core/eastwood/libew-sink.a',
        '../../build/osx-x86_64-release/eastwood-core/eastwood/libew-ffmpeg.a',
        '../../build/osx-x86_64-release/eastwood-core/eastwood/libew-subscribe.a',
        '../../build/osx-x86_64-release/eastwood-core/eastwood/libew-stats.a',
        '../../build/osx-x86_64-release/ffmpeg/build/x86_64/libavformat/libavformat.a',
        '../../build/osx-x86_64-release/ffmpeg/build/x86_64/libavcodec/libavcodec.a',
        '../../build/osx-x86_64-release/ffmpeg/build/x86_64/libswscale/libswscale.a',
        '../../build/osx-x86_64-release/ffmpeg/build/x86_64/libswresample/libswresample.a',
        '../../build/osx-x86_64-release/ffmpeg/build/x86_64/libavfilter/libavfilter.a',
        '../../build/osx-x86_64-release/ffmpeg/build/x86_64/libavutil/libavutil.a',
        '../../build/osx-x86_64-release/x264/build/x86_64/libx264.a',
        '../../build/osx-x86_64-release/rtmpdump/build/x86_64/lib/librtmp.a',
        '../../build/osx-x86_64-release/freetype/libfreetype.a',
        '../../build/osx-x86_64-release/boost/libat_boost_execution_monitor.a',
        '../../build/osx-x86_64-release/boost/libat_boost_random.a',
        '../../build/osx-x86_64-release/boost/libat_boost_system.a',
        '../../build/osx-x86_64-release/boost/libat_boost_atomic.a',
        '../../build/osx-x86_64-release/boost/libat_boost_log.a',
        '../../build/osx-x86_64-release/boost/libat_boost_filesystem.a',
        '../../build/osx-x86_64-release/boost/libat_boost_regex.a',
        '../../build/osx-x86_64-release/boost/libat_boost_program_options.a',
        '../../build/osx-x86_64-release/boost/libat_boost_thread.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libcng.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libg711.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libg722.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libpcm16b.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libred.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libaudio_coding_module.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libaudio_conference_mixer.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libaudio_decoder_interface.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libaudio_encoder_interface.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libaudio_device.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libaudio_processing.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libaudio_processing_sse2.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libbitrate_controller.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/common_audio/libcommon_audio.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/common_audio/libcommon_audio_sse2.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/common_video/libcommon_video.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libcongestion_controller.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/tools/libframe_editing_lib.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libilbc.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libisac.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libisac_common.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libisac_fix.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/base/librtc_base.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/base/librtc_base_approved.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/librtc_event_log.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/media/librtc_media.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/p2p/librtc_p2p.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/pc/librtc_pc.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/librent_a_codec.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/api/libjingle_peerconnection.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/sound/librtc_sound.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/p2p/libstunprober.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libmedia_file.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libneteq.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/third_party/openmax_dl/dl/libopenmax_dl.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libpaced_sender.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libremote_bitrate_estimator.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/librtp_rtcp.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/system_wrappers/libsystem_wrappers.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/system_wrappers/libfield_trial_default.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/system_wrappers/libmetrics_default.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/third_party/usrsctp/libusrsctplib.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libvideo_capture_module.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libvideo_capture.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/video_coding/utility/libvideo_coding_utility.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libvideo_processing.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libvideo_processing_sse2.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libvideo_render.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libvideo_render_module.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/voice_engine/libvoice_engine.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/libwebrtc.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/libwebrtc_common.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libwebrtc_i420.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libwebrtc_opus.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libwebrtc_utility.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libwebrtc_video_coding.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/video_coding/codecs/vp8/libwebrtc_vp8.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/video_coding/codecs/vp9/libwebrtc_vp9.a',
        '../../build/osx-x86_64-release/third_party/webrtc/osx/Debug/obj.target/webrtc/modules/libwebrtc_h264.a',
        '../../build/osx-x86_64-release/jangle/jangle/libjangle-webrtc.a',
        '../../build/osx-x86_64-release/third_party/libsrtp/build/x86_64/lib/libsrtp.a',
        '../../build/osx-x86_64-release/libyuv/libyuv.a',
        '../../build/osx-x86_64-release/libvpx/x86_64/libvpx.a',
        '../../build/osx-x86_64-release/opus/build/x86_64/.libs/libopus.a',
        '../../build/osx-x86_64-release/protobuf/src/cmake/libprotobuf.a',
        '../../build/osx-x86_64-release/boringssl/src/ssl/libssl.a',
        '../../build/osx-x86_64-release/boringssl/src/decrepit/libdecrepit.a',
        '../../build/osx-x86_64-release/boringssl/src/crypto/libcrypto.a',
        '../../build/osx-x86_64-release/jsoncpp/lib_json/libjsoncpp.a'
      ],

      'configurations': {
        'Debug': {
          'defines': [ 'DEBUG', '_DEBUG', 'DEBUG_'
          ]
      },

        'Release': {
          'defines': [ 'NDEBUG' 
          ]
        }
      },
  
      "include_dirs": [
        # relative path from the project top directory
        "../at-deps/mediacore",
        "../at-deps/libtecate",
        "../at-deps/carmel/include",
        "../at-deps/eastwood-core",
        "../at-deps/third_party/boost/src",
        "node_modules/node-media-utils/src"
      ],

      'conditions': [
        [
          'OS=="mac"', {
            'xcode_settings': {
              'GCC_ENABLE_CPP_RTTI': 'YES',
              'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
              'OTHER_CPLUSPLUSFLAGS' : [
                '-std=c++14','-stdlib=libc++',
                '-Qunused-arguments', '-Wno-unused-parameter', '-Wno-unused-function', '-Wno-shorten-64-to-32', '-Wno-attributes',
                '-Wthread-safety', '-Wno-unused-local-typedef', '-Wno-deprecated-register', '-Wno-unused-const-variable',
                '-DAT_USE_BOOST_MUTEX',
                '-DAT_USE_BOOST_THREAD',
                # '-DHAVE_OPENSSL_SSL_H',
                '-DAT_ENABLE_THREAD_ANNOTATIONS', '-DAT_USE_LOCK_STYLE_THREAD_SAFETY_ATTRIBUTES',
              ],
              'OTHER_LDFLAGS': [
                '-stdlib=libc++',
                '-bundle', '-undefined dynamic_lookup', '-Wl,-no_pie', '-arch x86_64',
                '-framework VideoToolbox', '-framework VideoDecodeAcceleration', '-framework Security',
                '-framework CoreServices', '-framework OpenGL', '-framework QTKit', '-framework Cocoa', '-framework AudioToolbox',
                '-framework CoreAudio', '-framework IOKit', '-framework AVFoundation', '-framework CoreMedia', '-framework CoreVideo',
                '-framework Foundation'
              ],
              'MACOSX_DEPLOYMENT_TARGET': '10.12'
            },
            'defines': [
              'AT_OSX=1'
            ]
          }
        ],
        [
          'OS=="linux"', {
            'defines': [
              'LINUX',
              'HASH_NAMESPACE=__gnu_cxx',
              'WEBRTC_LINUX'
            ],
            
            'include_dirs': [
            ],

            'cflags': [
              '-fPIC',
            ],

            'libraries': [
            ]
          }
        ]
      ]
    }
  ]
}

