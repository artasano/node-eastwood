// both ok
// var EastWood = require('../libs/eastwood').EastWood;
var EastWood = require('../libs/index').EastWood;

// EastWood(100);

const obj = new EastWood(10);
console.log('Returned ' + obj.add(3, function(val) { console.log('Callback ' + val); }));

const obj2 = new EastWood(20);
console.log('Returned ' + obj2.add(3, function(val) { console.log('Callback ' + val); }));

process.exit(0);
