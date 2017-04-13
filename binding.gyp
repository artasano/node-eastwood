{
  "targets": [
    {
      "target_name": "eastwood_addon",
      "sources": [
        "src/eastwood_addon.cc",
        "src/eastwood.cc"
      ],

      # 'defines' does not work here
      # 'defines': [
      #   'AT_ENABLE_THREAD_ANNOTATIONS', 'AT_USE_LOCK_STYLE_THREAD_SAFETY_ATTRIBUTES'
      # ],

      # note: needs a patch with /usr/local/lib/node_modules/node-gyp/gyp/pylib/gyp/generator/make.py for cflags to take effect on Mac
      'cflags_cc': [
        '-std=c++14', '-g', '-stdlib=libc++', '-pipe',
        '-Qunused-arguments', '-Wno-unused-parameter', '-Wno-unused-function', '-Wno-shorten-64-to-32', '-Wno-attributes',
        '-Wthread-safety', '-Wno-unused-local-typedef', '-Wno-deprecated-register', '-Wno-unused-const-variable',
        '-Wno-unknown-warning-option', '-fstack-protector',
        '-arch x86_64',
        '-DAT_ENABLE_THREAD_ANNOTATIONS', '-DAT_USE_LOCK_STYLE_THREAD_SAFETY_ATTRIBUTES',
        '-DAT_USE_BOOST_MUTEX',
        '-DAT_USE_BOOST_THREAD',
        '-DBOOST_SYSTEM_NO_DEPRECATED=1',
        '-DCARBON_DEPRECATED=YES',
        '-DCHROMIUM_BUILD',
        '-DCLD_VERSION=2',
        '-DDYNAMIC_ANNOTATIONS_ENABLED=1',
        '-DENABLE_APP_LIST=1',
        '-DENABLE_AUTOFILL_DIALOG=1',
        '-DENABLE_BACKGROUND=1',
        '-DENABLE_BASIC_PRINTING=1',
        '-DENABLE_CAPTIVE_PORTAL_DETECTION=1',
        '-DENABLE_CONFIGURATION_POLICY',
        '-DENABLE_EXTENSIONS=1',
        '-DENABLE_MEDIA_ROUTER=1',
        '-DENABLE_NOTIFICATIONS',
        '-DENABLE_PDF=1',
        '-DENABLE_PEPPER_CDMS',
        '-DENABLE_PLUGINS=1',
        '-DENABLE_PLUGIN_INSTALLATION=1',
        '-DENABLE_PRINTING=1',
        '-DENABLE_PRINT_PREVIEW=1',
        '-DENABLE_SERVICE_DISCOVERY=1',
        '-DENABLE_SESSION_SERVICE=1',
        '-DENABLE_SETTINGS_APP=1',
        '-DENABLE_SPELLCHECK=1',
        '-DENABLE_SUPERVISED_USERS=1',
        '-DENABLE_TASK_MANAGER=1',
        '-DENABLE_THEMES=1',
        '-DENABLE_TOPCHROME_MD=1',
        '-DENABLE_WEBRTC=1',
        '-DEXPAT_RELATIVE_PATH',
        '-DFEATURE_ENABLE_SSL',
        '-DFIELDTRIAL_TESTING_ENABLED',
        '-DFULL_SAFE_BROWSING',
        '-DHASH_NAMESPACE=__gnu_cxx',
        '-DHAVE_OPENSSL_SSL_H',
        '-DHAVE_WEBRTC_VIDEO',
        '-DLIBPEERCONNECTION_IMPLEMENTATION',
        '-DLIBPEERCONNECTION_LIB',
        '-DLOGGING=1',
        '-DPOSIX',
        '-DWEBRTC_POSIX',
        '-DSAFE_BROWSING_CSD',
        '-DSAFE_BROWSING_DB_LOCAL',
        #'-DSSL_USE_OPENSSL',
        '-DUSE_BROWSER_SPELLCHECKER=1',
        '-DUSE_LIBJPEG_TURBO=1',
        '-DUSE_LIBPCI=1',
        #'-DUSE_OPENSSL=1',
        '-DV8_DEPRECATION_WARNINGS',
        '-DV8_USE_EXTERNAL_STARTUP_DATA',
        '-DWEBRTC_EXTERNAL_JSON',
        '-DWEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE',
        '-DWTF_USE_DYNAMIC_ANNOTATIONS=1',
        '-D_REENTRANT',
        '-D__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORE=0',
        '-D__STDC_CONSTANT_MACROS',
        '-D__STDC_FORMAT_MACROS',
        #tiral
        "-isystem../at-deps/third_party/boringssl/src/include"
      ],

      'ldflags': [
        '-search_paths_first', '-headerpad_max_install_names'
      ],

      'libraries': [
        '-lpthread',
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
        '../../build/osx-x86_64-release/eastwood-core/eastwood/libew-common.a',
        '../../build/osx-x86_64-release/eastwood-core/eastwood/libew-source.a',
        '../../build/osx-x86_64-release/eastwood-core/eastwood/libew-sink.a',
        '../../build/osx-x86_64-release/eastwood-core/eastwood/libew-ffmpeg.a',
        '../../build/osx-x86_64-release/eastwood-core/eastwood/libew-subscribe.a',
        '../../build/osx-x86_64-release/eastwood-core/ffmpeg/build/x86_64/libavformat/libavformat.a',
        '../../build/osx-x86_64-release/eastwood-core/ffmpeg/build/x86_64/libavcodec/libavcodec.a',
        '../../build/osx-x86_64-release/eastwood-core/ffmpeg/build/x86_64/libswscale/libswscale.a',
        '../../build/osx-x86_64-release/eastwood-core/ffmpeg/build/x86_64/libswresample/libswresample.a',
        '../../build/osx-x86_64-release/eastwood-core/ffmpeg/build/x86_64/libavfilter/libavfilter.a',
        '../../build/osx-x86_64-release/eastwood-core/ffmpeg/build/x86_64/libavutil/libavutil.a',
        '../../build/osx-x86_64-release/eastwood-core/x264/build/x86_64/libx264.a',
        '../../build/osx-x86_64-release/eastwood-core/rtmpdump/build/x86_64/lib/librtmp.a',
        '../../build/osx-x86_64-release/eastwood-core/freetype/libfreetype.a',
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
        "src",
        # relative path from the project top directory
        "../at-deps/mediacore",
        "../at-deps/libtecate",
        "../at-deps/carmel/include",
        "../at-deps/eastwood-core",
        "../at-deps/third_party/boost/src",
        "../at-deps/third_party/ffmpeg/src",
        "../at-deps/ffmpeg/build/x86_64",
        "../at-deps/third_party/jsoncpp/include",
        # trying isystem "../at-deps/third_party/boringssl/src/include"
      ],

      'defines':  [
      ],
      
      'conditions': [
        [
          'OS=="mac"',
          {
            'xcode_settings': {
              'GCC_ENABLE_CPP_RTTI': 'YES',
              'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
              'OTHER_CFLAGS': [
                '-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk',
                '-mmacosx-version-min=10.9',
                '-framework VideoToolbox', '-framework VideoDecodeAcceleration', '-framework Security',
                '-framework CoreServices', '-framework OpenGL', '-framework QTKit', '-framework Cocoa', '-framework AudioToolbox',
                '-framework CoreAudio', '-framework IOKit', '-framework AVFoundation', '-framework CoreMedia', '-framework CoreVideo',
                '-framework Foundation'
              ],
              'OTHER_LDFLAGS': [
              ]
            },
            
            'cflags': [
            ],

            'ldflags': [
              '-L../../build/osx-x86_64'
            ],

            'include_dirs': [
            ],
            
            'libraries': [
              '/usr/lib/libz.dylib'
           ],

            'defines': [
              'OSX',
              'AT_OSX=1',
              'OSX',
              'WEBRTC_MAC'
            ]

          },
        ],
        [
          'OS=="linux"',
          {
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

