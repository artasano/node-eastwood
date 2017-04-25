
var EventCallbackClass;
try {
  EventCallbackClass = require('../build/Debug/addon_util_test').EventCallbackClass;
} catch (e) {
  if ('MODULE_NOT_FOUND' === e.code) {
    EventCallbackClass = require('../build/Release/addon_util_test').EventCallbackClass;
  } else {
    throw e;
  }
}

var assert = function(cond) {
  if (!cond) throw 'failed';
}

var invoker = new   EventCallbackClass();
invoker.runAsync(function(err, val) {
  assert(!err);
  assert(val === 101);

  invoker.stop();  // need to stop, otherwise the test does not end
  // done();
  console.log('done');
})
// invoker.callbackInJSThread(100);
invoker.callbackInWorkerThread(100);

