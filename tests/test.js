process.on('uncaughtException', function(exception) {
  console.log('Uncaught Exception: ' + exception + '\n' + exception.stack);
  process.exit(1);
});

// both ok
// var EastWood = require('../libs/eastwood').EastWood;
var EastWood = require('../libs/index').EastWood;

const ew = new EastWood();

const s1 = ew.createSubscriber(10);
// also works
// var Subscriber = require('../libs/index').Subscriber;
// const s1 = new Subscriber(10);
console.log('Returned '
+ s1.add(3, function(sub) {
    console.log('Callback ' + sub.getValue());
  }));

const s2 = ew.createSubscriber(20);
console.log('Returned ' + s2.add(3, function(sub) {
    console.log('Callback ' + sub.getValue());
    console.log('Throwing');
    throw new Error('Test Exception');
  }));

process.exit(0);
