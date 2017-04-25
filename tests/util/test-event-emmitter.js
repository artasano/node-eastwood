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


describe('EventEmitter', function() {

  beforeEach(function(done) {
    done();
  });

  it('emit in JS thread', function(done) {
    var emitter = new   EventCallbackClass();
    emitter.on('event1', function(val) {
      expect(val).to.equal('aaa event1');

      emitter.stop();  // need to stop, otherwise the test does not end
      done();
    })
    emitter.fireEvent1InJSThread('aaa');
  });

  it('emit in worker thread', function(done) {
    var emitter = new   EventCallbackClass();
    emitter.on('event2', function(val) {

console.log("Got 2");

      expect(val).to.equal('aaa event2');

      emitter.stop();  // need to stop, otherwise the test does not end
      done();
console.log("Done 2");
    })
    emitter.fireEvent2InWorkerThread('aaa');

console.log("Fired 2");

  });

  it('emit accordingly', function(done) {
    var emitter = new   EventCallbackClass();
    var got1 = false;
    var got2 = false;
    emitter.on('event1', function(val) {
      expect(val).to.equal('aaa event1');
      if (got2) {
        emitter.stop();  // need to stop, otherwise the test does not end
        done();
      } else {
        got1 = true;
      }
    })
    emitter.on('event2', function(val) {
      expect(val).to.equal('bbb event2');
      if (got1) {
        emitter.stop();  // need to stop, otherwise the test does not end
        done();
      } else {
        got2 = true;
      }
    })
    emitter.fireEvent2InWorkerThread('bbb');
    emitter.fireEvent1InJSThread('aaa');
  });
});
