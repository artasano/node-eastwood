/// @copyright © 2017 Airtime Media.  All rights reserved.

var chai = require('chai');
var expect = chai.expect;
chai.config.includeStack = true;

var util = require('node-media-utils').util;

var EventCallbackClass;
try {
  EventCallbackClass = require('./build/Debug/addon_util_test').EventCallbackClass;
} catch (e) {
  if ('MODULE_NOT_FOUND' === e.code) {
    EventCallbackClass = require('./build/Release/addon_util_test').EventCallbackClass;
  } else {
    throw e;
  }
}


describe('CallbackInvoker', function() {

  beforeEach(function(done) {
    done();
  });

  it('callback in JS thread', function(done) {
    var invoker = new   EventCallbackClass();
    invoker.runAsync(function(err, val) {
      expect(err).to.be.not.ok;
      expect(val).to.equal(101);

      invoker.stop();  // need to stop, otherwise the test does not end
      done();
    })
    invoker.callbackInJSThread(100);
  });

  it('callback in worker thread', function(done) {
    var invoker = new   EventCallbackClass();
    invoker.runAsync(function(err, val) {
      expect(err).to.be.not.ok;
      expect(val).to.equal(101);

      invoker.stop();  // need to stop, otherwise the test does not end
      done();
    })
    invoker.callbackInWorkerThread(100);
  });

  it('callback with error', function(done) {
    var invoker = new   EventCallbackClass();
    invoker.runAsync(function(err, val) {
      expect(err.message).to.equal('aaa exception');

      invoker.stop();  // need to stop, otherwise the test does not end
      done();
    })
    invoker.callbackErrorInWorkerThread('aaa');
  });
});
