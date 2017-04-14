var eastwood;
try {
  eastwood = require('../build/Debug/eastwood_addon');
} catch(ex) {
  eastwood = require('../build/Release/eastwood_addon');
}
exports.EastWood = eastwood.EastWood;
// TODO(Art): figure out how to refer to enum in there
// exports.EastWood.AudioSinkNone = 0;
// exports.EastWood.AudioSinkFile = 1;
// exports.EastWood.AudioSinkFFMpeg = 2;
// exports.EastWood.VideoSinkNone = 0;
// exports.EastWood.VideoSinkFile = 1;
// exports.EastWood.VideoSinkFFMpeg = 2;

exports.Subscriber = eastwood.Subscriber;
