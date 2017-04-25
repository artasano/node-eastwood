/// @copyright © 2017 Airtime Media.  All rights reserved.

var chai = require('chai');
var expect = chai.expect;
chai.config.includeStack = true;

var util = require('node-media-utils').util;

var AddonClass;
try {
  AddonClass = require('./build/Debug/addon_util_test').AddonClass;
} catch (e) {
  if ('MODULE_NOT_FOUND' === e.code) {
    EventCallbackClass = require('./build/Release/addon_util_test').EventCallbackClass;
  } else {
    throw e;
  }
}


describe('addon_util-functions', function() {

  describe('util-constatnts', function() {
    if('should have all constants', function() {
      expect(AddonClass.Enum1).to.equal(0);
      expect(AddonClass.Enum2).to.equal(1);
      expect(AddonClass.IntConstant1).to.equal(10);
      expect(AddonClass.UintConstant1).to.equal(11);
      expect(AddonClass.FloatConstant1).to.equal(1.1);
      expect(AddonClass.DoubleConstant1).to.equal(1.2);
      expect(AddonClass.BoolConstantT).to.equal(true);
      expect(AddonClass.BoolConstantF).to.equal(false);
      expect(AddonClass.CharConstant1).to.equal('x');
      expect(AddonClass.StringConstant1).to.equal('abc');
    });
  });

  describe('util-functions', function() {
    beforeEach(function(done) {
      done();
    });

    it('should convert data between C++ and V8', function() {
      var a = new AddonClass();
      expect(a.testConversionToFromV8()).to.be.true;
    });

    it('should throw on conversion error', function() {
      var a = new AddonClass();
      expect(a.testConversionFailure()).to.be.true;
    });

    it('should throw if arg is given to no-arg func', function() {
      var a = new AddonClass();
      try {
        a.testCheckArgs0('extra');
        expect(false).to.be.ok;
      } catch (e) {
        expect(e.toString()).to.contain('testCheckArgs0');
        expect(e.toString()).to.contain('Takes 0');
        expect(e.toString()).to.contain('given 1');
      }
    });

    it('should be ok with correct num args 0', function() {
      var a = new AddonClass();
      a.testCheckArgs0();
      expect(true).to.be.ok;  // ok if not thrown
    });

    it('should accept no-arg given to optinal arg', function() {
      var a = new AddonClass();
      a.testCheckArgs0or1();
      expect(true).to.be.ok;  // ok if not thrown
    });

    it('should accept correct-arg given to optinal arg', function() {
      var a = new AddonClass();
      a.testCheckArgs0or1(()=>{ console.log('ok'); });
      expect(true).to.be.ok;  // ok if not thrown
    });

    it('should throw if incorrect arg type is given to an optional arg', function() {
      var a = new AddonClass();
      try {
        a.testCheckArgs0or1('not-a-function');
        expect(false).to.be.ok;
      } catch (e) {
        expect(e.toString()).to.contain('testCheckArgs0or1');
        expect(e.toString()).to.contain('Wrong argument at 0');
        expect(e.toString()).to.contain('given not-a-function');
      }
    });

    it('should throw if extra arg is given to an optional arg', function() {
      var a = new AddonClass();
      try {
        a.testCheckArgs0or1(()=>{ console.log('ok'); }, 'extra arg');
        expect(false).to.be.ok;
      } catch (e) {
        expect(e.toString()).to.contain('testCheckArgs0or1');
        expect(e.toString()).to.contain('Takes 1');
        expect(e.toString()).to.contain('given 2');
      }
    });

    it('should throw if insufficient number of args were given', function() {
      var a = new AddonClass();
      try {
        a.testCheckArgs2(0.1);
        expect(false).to.be.ok;
      } catch (e) {
        expect(e.toString()).to.contain('testCheckArgs2');
        expect(e.toString()).to.contain('Needs 2');
        expect(e.toString()).to.contain('given 1');
      }
    });

    it('should throw if extra number of args were given', function() {
      var a = new AddonClass();
      try {
        a.testCheckArgs2(0.1, 2, 3);
        expect(false).to.be.ok;
      } catch (e) {
        expect(e.toString()).to.contain('testCheckArgs2');
        expect(e.toString()).to.contain('Takes 2');
        expect(e.toString()).to.contain('given 3');
      }
    });

    it('should throw if wrong type of arg was given', function() {
      var a = new AddonClass();
      try {
        a.testCheckArgs2(0.1, 'not-a-number');
        expect(false).to.be.ok;
      } catch (e) {
        expect(e.toString()).to.contain('testCheckArgs2');
        expect(e.toString()).to.contain('Wrong argument at 1');
        expect(e.toString()).to.contain('given not-a-number');
      }
    });

    it('should throw custom error message', function() {
      var a = new AddonClass();
      try {
        a.testCheckArgs2(0.1, 10);
        expect(false).to.be.ok;
      } catch (e) {
        expect(e.toString()).to.contain('testCheckArgs2');
        expect(e.toString()).to.contain('Must be less than 10');
      }
    });

    it('should accept correct 2 args', function() {
      var a = new AddonClass();
      a.testCheckArgs2(0.1, 9);
      expect(true).to.be.ok;  // ok if not thrown
    });

  });
});