var eastwood;
try {
  eastwood = require('../build/Debug/eastwood_addon');
} catch(ex) {
  eastwood = require('../build/Release/eastwood_addon');
}
exports.EastWood = eastwood.EastWood;
exports.Subscriber = eastwood.Subscriber;
