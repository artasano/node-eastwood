/// @copyright © 2017 Airtime Media.  All rights reserved.

var eastwood;
try {
  eastwood = require('../build/Debug/eastwood_addon');
} catch(ex) {
  if ('MODULE_NOT_FOUND' === e.code) {
    eastwood = require('../build/Release/eastwood_addon');
  } else {
    throw e;
  }
}
exports.EastWood = eastwood.EastWood;
