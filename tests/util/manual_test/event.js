/// @copyright © 2017 Airtime Media.  All rights reserved.

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


var emitter = new   EventCallbackClass();
var got1 = false;
var got2 = false;


got1 = true;

// emitter.on('event1', function(val) {

// console.log("Got 1 " + val);

// //  expect(val).to.equal('aaa event');

//   if (got2) {
//     emitter.stop();  // need to stop, otherwise the test does not end
// //  done();
//     console.log("Done by 1");
//   } else {
//     got1 = true;
//   }
// });


emitter.on('event2', function(val) {

console.log("Got 2 " + val);

//  expect(val).to.equal('aaa event');

  if (got1) {
    emitter.stop();  // need to stop, otherwise the test does not end
//  done();
    console.log("Done by 2");
  } else {
    got2 = true;
  }
});

emitter.fireEvent2InWorkerThread('aaa');
// emitter.fireEvent1InJSThread('aaa');
// setTimeout(function() {
//     emitter.stop();  // need to stop, otherwise the test does not end
// }, 2000);
