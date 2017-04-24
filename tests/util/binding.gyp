{
  "targets": [
    {
     "target_name": "addon_util_test",
     "sources": [
       "src/test_addon.cc",
       "src/addon_class.cc",
       "src/event_callback_class.cc",
       "../../src/util/addon_util.cc"
     ],

      'libraries': [
        # relative path from build/ directory
      ],

      "include_dirs": [
        # relative path from the project top directory
        "../../src",
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
                '-DAT_ENABLE_THREAD_ANNOTATIONS', '-DAT_USE_LOCK_STYLE_THREAD_SAFETY_ATTRIBUTES',
              ],
              'OTHER_LDFLAGS': [
                '-stdlib=libc++',
                '-bundle', '-undefined dynamic_lookup', '-Wl,-no_pie', '-arch x86_64',
#                '-framework VideoToolbox', '-framework VideoDecodeAcceleration', '-framework Security',
#                '-framework CoreServices', '-framework OpenGL', '-framework QTKit', '-framework Cocoa', '-framework AudioToolbox',
#                '-framework CoreAudio', '-framework IOKit', '-framework AVFoundation', '-framework CoreMedia', '-framework CoreVideo',
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
