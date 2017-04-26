console.log('0');

var EastWood = require('../libs/index').EastWood;


// const ew = new EastWood(EastWood.LogLevel_Debug, true, false);
const ew = new EastWood(EastWood.LogLevel_Fatal, true, false);
const s = ew.createSubscriber();

// s.configure().duration('00:01:23', 234);
// console.log('4');
// console.log(s.configure().toObject());


// const c = s.configuration()
//            .bixby('host1', 10).streamUrl('surl2').userId('uuu');
// try {
//   c.verify();
// } catch (ex) {
//   console.log('Exception: ' + ex);
// }


const c = s.configuration()
  // set other mandatory items
  .streamNotifier('host', 123, 'tag', true, true).userId('uuu').duration('infinite')
  .sink({ sink: EastWood.AudioSink_None }, { sink: EastWood.VideoSink_None });
try {
  c.verify();
} catch (ex) {
  console.log('Exception: ' + ex);
}
