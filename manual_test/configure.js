console.log('0');

var EastWood = require('../libs/index').EastWood;

console.log('1');
const ew = new EastWood(EastWood.LogLevel_Debug, true, false);
console.log('2');
const s = ew.createSubscriber();
console.log('3');
// s.configure().duration('00:01:23', 234);
// console.log('4');
// console.log(s.configure().toObject());

const c = s.configuration()
           .bixby('host1', 10).streamUrl('surl2').userId('uuu');

  c.verify();
