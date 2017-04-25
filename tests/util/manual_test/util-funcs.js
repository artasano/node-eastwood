
var AddonClass;
try {
  AddonClass = require('../build/Debug/addon_util_test').AddonClass;
} catch (e) {
  if ('MODULE_NOT_FOUND' === e.code) {
    EventCallbackClass = require('./build/Release/addon_util_test').EventCallbackClass;
  } else {
    throw e;
  }
}

var assert = function(cond) {
  if (!cond) throw 'failed';
}

var a = new AddonClass();
a.testCheckArgs0or1();
