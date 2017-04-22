var EastWood = require('../libs/index').EastWood;

var ew = new EastWood(EastWood.LogLevel_Debug, true, false);  // console-log, no-syslog
var sub = ew.createSubscriber()
sub.configuration()
  .bixbyAllocator('media-allocator.eng.airtime.com', 443, 'at-west2')
  .userId('art123')
  .streamNotifier('stream-notifier.eng.signal.is', 3301, 'arttest', true, true)  // TLS, cert
  .authSecret('hello@test-eng')
  .audioSink(EastWood.AudioSink_None)
  .videoSink(EastWood.VideoSink_None)
  .duration('00:00:30');
console.log(sub.configuration().toObject());

sub.on('error', function(err) {
  console.log('Error: ' + err);
});

//        sub.start();
sub.stop((result) => {
  console.log('Stopped ' + result);
  sub = null;
// TODO(Art): tmp
//  process.exit(0);
});

// setTimeout(()=>{
//   console.log('Finished');
// }, 5000);
