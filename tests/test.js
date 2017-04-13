process.on('uncaughtException', function(exception) {
  console.log('Uncaught Exception: ' + exception + '\n' + exception.stack);
  process.exit(1);
});

// both ok
// var EastWood = require('../libs/eastwood').EastWood;
var EastWood = require('../libs/index').EastWood;

const ew = new EastWood();

const s1 = ew.createSubscriber();
var x = s1.configure()
  .bixby('11.22.33.44', 55)
  .streamNotifier('2.3.4.5', 66, 'tagABC', false, true)
  .audioSink(EastWood.AudioSinkFile, 'some/place/audio.dat')
  .videoSink(EastWood.VideoSinkFile, 'other/place/video.dat')
  .subscriptionErrorRetry(10, 200, 3.0);

console.log('Subscriber config:\n' + s1.dumpConfig());

// don't want it to work
// var Subscriber = require('../libs/index').Subscriber;
// const s = new Subscriber();


process.exit(0);
