var EastWood = require('../libs/index').EastWood;

        const ew = new EastWood(EastWood.LogLevel_Debug, true, false);  // console-log, no-syslog
        const sub = ew.createSubscriber()
        sub.configuration()
          .bixbyAllocator('media-allocator.eng.airtime.com', 8192, 'at-west')
          .userId('art123')
          .streamNotifier('stream-notifier.eng.signal.is', 443, 'arttest', true, true)  // TLS, cert
          .authSecret('hello@test-eng')
//          .sink({ sink: EastWood.AudioSink_None }, { sink: EastWood.VideoSink_None })
          // .ffmpegSink("/opt/git/node-eastwood/output/z.mp4", 
          //   "ffmpeg_resource_dir=/opt/git/eastwood/eastwood/resources " +
          //   "ffmpeg_target_video_bitrate=500000 ffmpeg_target_audio_bitrate=128000 " +
          //   "ffmpeg_target_width=720 ffmpeg_target_height=720 ffmpeg_target_fps=30 " +
          //   "hls_time=2 hls_list_size=2 hls_wrap=3")
          .ffmpegSink("rtmp://1.2.3.4", 
            "ffmpeg_resource_dir=/opt/git/at-deps/eastwood-core/eastwood/resources " +
            "ffmpeg_output_format=flv ffmpeg_force_video_codec=h264 ffmpeg_force_audio_codec=aac")
          .duration('00:00:10')
          .printFrameInfo(true);
        console.log(sub.configuration().toObject());

        sub.on("finish", function(err) {
          console.log("Finish: " + err);
          sub.stop(function(result) {
            console.log("Stopped: " + result);
          });
        });

        sub.start();

//         setTimeout(()=>{
//           console.log("Finished");
//           sub.stop(function(result) {
//             console.log("Stopped: " + result);
// //            done();
//           });
//         }, 15000);
