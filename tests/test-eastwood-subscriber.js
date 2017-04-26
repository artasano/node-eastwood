/// @copyright © 2017 Airtime Media.  All rights reserved.

var chai = require('chai');
var expect = chai.expect;
chai.config.includeStack = true;

var util = require('node-media-utils').util;

var EastWood = require('../libs/index').EastWood;

var testLogLevel = EastWood.LogLevel_Fatal;

describe('EastWood', function() {

  beforeEach(function(done) {
    done();
  });

  it('should throw if insufficient args are given in new', function() {
    try {
      new EastWood();
      expect(false).to.be.ok;
    } catch (e) {
      expect(e.toString()).to.contain('EastWood');
      expect(e.toString()).to.contain('Needs 3 args but given 0');
    }
    try {
      new EastWood(EastWood.LogLevel_Info);
      expect(false).to.be.ok;
    } catch (e) {
      expect(e.toString()).to.contain('EastWood');
      expect(e.toString()).to.contain('Needs 3 args but given 1');
    }
    try {
      new EastWood(EastWood.LogLevel_Info, true);
      expect(false).to.be.ok;
    } catch (e) {
      expect(e.toString()).to.contain('EastWood');
      expect(e.toString()).to.contain('Needs 3 args but given 2');
    }
  });
  it('should throw if extra args are given in new', function() {
    try {
      new EastWood(EastWood.LogLevel_Info, true, true, 'log.props', 123);
      expect(false).to.be.ok;
    } catch (e) {
      expect(e.toString()).to.contain('EastWood');
      expect(e.toString()).to.contain('Takes 4 args but given 5');
    }
  });
  it('should throw if incorrect arg is given in new', function() {
    try {
      new EastWood(100, true, true, 'log.props');
      expect(false).to.be.ok;
    } catch (e) {
      expect(e.toString()).to.contain('EastWood');
      expect(e.toString()).to.contain('Incorrect log level value 100');
    }
    try {
      new EastWood('x', true, true, 'log.props');
      expect(false).to.be.ok;
    } catch (e) {
      expect(e.toString()).to.contain('EastWood');
      expect(e.toString()).to.contain('Wrong argument at 0');
      expect(e.toString()).to.contain('given x');
    }
    try {
      new EastWood(EastWood.LogLevel_Debug, 'x', true, 'log.props');
      expect(false).to.be.ok;
    } catch (e) {
      expect(e.toString()).to.contain('EastWood');
      expect(e.toString()).to.contain('Wrong argument at 1');
      expect(e.toString()).to.contain('given x');
    }
    try {
      new EastWood(EastWood.LogLevel_Debug, false, 'x', 'log.props');
      expect(false).to.be.ok;
    } catch (e) {
      expect(e.toString()).to.contain('EastWood');
      expect(e.toString()).to.contain('Wrong argument at 2');
      expect(e.toString()).to.contain('given x');
    }
    try {
      new EastWood(EastWood.LogLevel_Debug, false, true, 123);
      expect(false).to.be.ok;
    } catch (e) {
      expect(e.toString()).to.contain('EastWood');
      expect(e.toString()).to.contain('Wrong argument at 3');
      expect(e.toString()).to.contain('given 123');
    }
  });
  it('should have valid enums', function() {
    expect(EastWood.LogLevel_Fatal).to.be.a('number');
    expect(EastWood.LogLevel_Error).to.be.a('number');
    expect(EastWood.LogLevel_Warning).to.be.a('number');
    expect(EastWood.LogLevel_Info).to.be.a('number');
    expect(EastWood.LogLevel_Debug).to.be.a('number');
    expect(EastWood.LogLevel_Fatal < EastWood.LogLevel_Error).to.be.true;
    expect(EastWood.LogLevel_Error < EastWood.LogLevel_Warning).to.be.true;
    expect(EastWood.LogLevel_Warning < EastWood.LogLevel_Info).to.be.true;
    expect(EastWood.LogLevel_Info < EastWood.LogLevel_Debug).to.be.true;

    expect(EastWood.AudioSink_None).to.be.a('number');
    expect(EastWood.AudioSink_File).to.be.a('number');
    expect(EastWood.AudioSink_None).to.not.equal(EastWood.AudioSink_File);

    expect(EastWood.VideoSink_None).to.be.a('number');
    expect(EastWood.VideoSink_File).to.be.a('number');
    expect(EastWood.VideoSink_None).to.not.equal(EastWood.VideoSink_File);
  });
  describe('createSubscriber', function() {
    it('should create subscriber', function() {
      const ew = new EastWood(testLogLevel, true, false);
      const s = ew.createSubscriber();
      expect(s).to.be.ok;
      expect(s).to.be.an('object');
    });
    it('should throw if got extra arg', function() {
      const ew = new EastWood(testLogLevel, true, false);
      try {
        ew.createSubscriber(123);
        expect(false).to.be.ok;
      } catch (e) {
        expect(e.toString()).to.contain('EastWood');
        expect(e.toString()).to.contain('createSubscriber');
        expect(e.toString()).to.contain('Takes 0 args but given 1');
      }
    });
  });

  describe('Subscriber', function() {
    describe('Configuration', function() {
      describe('bixby', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.bixby();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('bixby');
            expect(e.toString()).to.contain('Needs 2');
            expect(e.toString()).to.contain('given 0');
          }
          try {
            c.bixby('host');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('bixby');
            expect(e.toString()).to.contain('Needs 2');
            expect(e.toString()).to.contain('given 1');
          }
        });
        it('should throw if given more args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.bixby('aaa', 22, 33);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('bixby');
            expect(e.toString()).to.contain('Takes 2');
            expect(e.toString()).to.contain('given 3');
          }
        });
        it('should throw if given incorrect args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.bixby(123, 456);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('bixby');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('given 123');
          }
          try {
            c.bixby('', 456);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('bixby');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('hostname cannot be empty');
          }
          try {
            c.bixby('host', 0);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('bixby');
            expect(e.toString()).to.contain('Wrong argument at 1');
            expect(e.toString()).to.contain('port number cannot be zero');
          }
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration()
                        .bixby('host-abc', 987)
                        .toObject();
          expect(c.bixby).to.be.ok;
          expect(c.bixby.host).to.equal('host-abc');
          expect(c.bixby.port).to.equal(987);
        });
      });

      describe('allocator', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.bixbyAllocator();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('bixbyAllocator');
            expect(e.toString()).to.contain('Needs 3');
            expect(e.toString()).to.contain('given 0');
          }
          try {
            c.bixbyAllocator('host');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('bixbyAllocator');
            expect(e.toString()).to.contain('Needs 3');
            expect(e.toString()).to.contain('given 1');
          }
          try {
            c.bixbyAllocator('host', 123);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('bixbyAllocator');
            expect(e.toString()).to.contain('Needs 3');
            expect(e.toString()).to.contain('given 2');
          }
        });
        it('should throw if given more args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.bixbyAllocator('aaa', 11, 'bbb', 22);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('bixbyAllocator');
            expect(e.toString()).to.contain('Takes 3');
            expect(e.toString()).to.contain('given 4');
          }
        });
        it('should throw if given incorrect args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.bixbyAllocator(123, 456, 'lll');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('bixbyAllocator');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('given 123');
          }
          try {
            c.bixbyAllocator('', 456, 'lll');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('bixbyAllocator');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('hostname cannot be empty');
          }
          try {
            c.bixbyAllocator('host', 'abc', 'lll');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('bixbyAllocator');
            expect(e.toString()).to.contain('Wrong argument at 1');
            expect(e.toString()).to.contain('given abc');
          }
          try {
            c.bixbyAllocator('host', 0, 'lll');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('bixbyAllocator');
            expect(e.toString()).to.contain('Wrong argument at 1');
            expect(e.toString()).to.contain('port number cannot be zero');
          }
          try {
            c.bixbyAllocator('host', 123, 456);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('bixbyAllocator');
            expect(e.toString()).to.contain('Wrong argument at 2');
            expect(e.toString()).to.contain('given 456');
          }
          try {
            c.bixbyAllocator('host', 123, '');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('bixbyAllocator');
            expect(e.toString()).to.contain('Wrong argument at 2');
            expect(e.toString()).to.contain('location cannot be empty');
          }
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration()
                        .bixbyAllocator('host-abc', 987, 'lll')
                        .toObject();
          expect(c.allocator).to.be.ok;
          expect(c.allocator.host).to.equal('host-abc');
          expect(c.allocator.port).to.equal(987);
          expect(c.allocator.loc).to.equal('lll');
        });
      });

      describe('streamNotifier', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.streamNotifier();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamNotifier');
            expect(e.toString()).to.contain('Needs 5');
            expect(e.toString()).to.contain('given 0');
          }
          try {
            c.streamNotifier('host');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamNotifier');
            expect(e.toString()).to.contain('Needs 5');
            expect(e.toString()).to.contain('given 1');
          }
          try {
            c.streamNotifier('host', 123);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamNotifier');
            expect(e.toString()).to.contain('Needs 5');
            expect(e.toString()).to.contain('given 2');
          }
          try {
            c.streamNotifier('host', 123, 'ttt');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamNotifier');
            expect(e.toString()).to.contain('Needs 5');
            expect(e.toString()).to.contain('given 3');
          }
          try {
            c.streamNotifier('host', 123, 'ttt', false);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamNotifier');
            expect(e.toString()).to.contain('Needs 5');
            expect(e.toString()).to.contain('given 4');
          }
        });
        it('should throw if given more args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.streamNotifier('aa', 1, 'bb', true, true, 2);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamNotifier');
            expect(e.toString()).to.contain('Takes 5');
            expect(e.toString()).to.contain('given 6');
          }
        });

        it('should throw if given incorrect args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.streamNotifier(123, 456, 'lll', false, true);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamNotifier');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('given 123');
          }
          try {
            c.streamNotifier('', 456, 'lll', false, true);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamNotifier');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('hostname cannot be empty');
          }
          try {
            c.streamNotifier('host', 'abc', 'lll', false, true);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamNotifier');
            expect(e.toString()).to.contain('Wrong argument at 1');
            expect(e.toString()).to.contain('given abc');
          }
          try {
            c.streamNotifier('host', 0, 'lll', false, true);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamNotifier');
            expect(e.toString()).to.contain('Wrong argument at 1');
            expect(e.toString()).to.contain('port number cannot be zero');
          }
          try {
            c.streamNotifier('host', 123, 456, false, true);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamNotifier');
            expect(e.toString()).to.contain('Wrong argument at 2');
            expect(e.toString()).to.contain('given 456');
          }
          try {
            c.streamNotifier('host', 123, '', false, true);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamNotifier');
            expect(e.toString()).to.contain('Wrong argument at 2');
            expect(e.toString()).to.contain('tag cannot be empty');
          }
          try {
            c.streamNotifier('host', 123, 'ttt', 456, true);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamNotifier');
            expect(e.toString()).to.contain('Wrong argument at 3');
            expect(e.toString()).to.contain('given 456');
          }
          try {
            c.streamNotifier('host', 123, 'ttt', true, 0.3);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamNotifier');
            expect(e.toString()).to.contain('Wrong argument at 4');
            expect(e.toString()).to.contain('given 0.3');
          }
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration()
                        .streamNotifier('host-abc', 987, 'ttt', false, true)
                        .toObject();
          expect(c.notifier).to.be.ok;
          expect(c.notifier.host).to.equal('host-abc');
          expect(c.notifier.port).to.equal(987);
          expect(c.notifier.tag).to.equal('ttt');
          expect(c.notifier.tls).to.equal(false);
          expect(c.notifier.cert).to.equal(true);
        });
      });

      describe('duration', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.duration();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('duration');
            expect(e.toString()).to.contain('Needs 1');
            expect(e.toString()).to.contain('given 0');
          }
        });
        it('should throw if given more args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.duration('00:00:01', 23);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('duration');
            expect(e.toString()).to.contain('Takes 1');
            expect(e.toString()).to.contain('given 2');
          }
        });
        it('should throw if given incorrect args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.duration(123);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('duration');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('given 123');
          }
          try {
            c.duration('not-a-duration');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('duration');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('not-a-duration');
          }
          try {
            c.duration('00:00:00');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('duration');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('duration must be longer than zero');
          }
          try {
            c.duration('-00:00:01');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('duration');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('duration must be longer than zero');
          }
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration()
                        .duration('00:01:23')
                        .toObject();
          expect(c.duration_ms).to.equal(83000);
        });
      });

      describe('userId', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.userId();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('userId');
            expect(e.toString()).to.contain('Needs 1');
            expect(e.toString()).to.contain('given 0');
          }
        });
        it('should throw if given more args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.userId('u1', 22);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('userId');
            expect(e.toString()).to.contain('Takes 1');
            expect(e.toString()).to.contain('given 2');
          }
        });
        it('should throw if given incorrect args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.userId(123);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('userId');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('given 123');
          }
          try {
            c.userId('');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('userId');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('user id cannot be empty');
          }
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration()
                        .userId('user123')
                        .toObject();
          expect(c.userId).to.equal('user123');
        });
      });

      describe('streamUrl', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.streamUrl();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamUrl');
            expect(e.toString()).to.contain('Needs 1');
            expect(e.toString()).to.contain('given 0');
          }
        });
        it('should throw if given more args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.streamUrl('s1', 22);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamUrl');
            expect(e.toString()).to.contain('Takes 1');
            expect(e.toString()).to.contain('given 2');
          }
        });
        it('should throw if given incorrect args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.streamUrl(123);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamUrl');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('given 123');
          }
          try {
            c.streamUrl('');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamUrl');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('stream URL cannot be empty');
          }
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration()
                        .streamUrl('s123')
                        .toObject();
          expect(c.streamURL).to.equal('s123');
        });
      });

      describe('certCheck', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.certCheck();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('certCheck');
            expect(e.toString()).to.contain('Needs 1');
            expect(e.toString()).to.contain('given 0');
          }
        });
        it('should throw if given more args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.certCheck(true, false);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('certCheck');
            expect(e.toString()).to.contain('Takes 1');
            expect(e.toString()).to.contain('given 2');
          }
        });
        it('should throw if given incorrect args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.certCheck(123);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('certCheck');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('given 123');
          }
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          c = ew.createSubscriber().configuration()
                        .certCheck(true)
                        .toObject();
          expect(c.cert).to.equal(true);
          c = ew.createSubscriber().configuration()
                        .certCheck(false)
                        .toObject();
          expect(c.cert).to.equal(false);
        });
      });

      describe('authSecret', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.authSecret();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('authSecret');
            expect(e.toString()).to.contain('Needs 1');
            expect(e.toString()).to.contain('given 0');
          }
        });
        it('should throw if given more args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.authSecret('aaa', 'bbb');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('authSecret');
            expect(e.toString()).to.contain('Takes 1');
            expect(e.toString()).to.contain('given 2');
          }
        });
        it('should throw if given incorrect args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.authSecret(123);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('authSecret');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('given 123');
          }
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration()
                        .authSecret('sec123')
                        .toObject();
          expect(c.secret).to.equal('sec123');
        });
      });

      describe('printFrameInfo', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.printFrameInfo();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('printFrameInfo');
            expect(e.toString()).to.contain('Needs 1');
            expect(e.toString()).to.contain('given 0');
          }
        });
        it('should throw if given more args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.printFrameInfo(false, true);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('printFrameInfo');
            expect(e.toString()).to.contain('Takes 1');
            expect(e.toString()).to.contain('given 2');
          }
        });
        it('should throw if given incorrect args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.printFrameInfo('aaa');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('printFrameInfo');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('given aaa');
          }
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          c = ew.createSubscriber().configuration()
                        .printFrameInfo(true)
                        .toObject();
          expect(c.frameInfo).to.equal(true);
          c = ew.createSubscriber().configuration()
                        .printFrameInfo(false)
                        .toObject();
          expect(c.frameInfo).to.equal(false);
        });
      });

      describe('sink', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.sink();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('sink');
            expect(e.toString()).to.contain('Needs 2');
            expect(e.toString()).to.contain('given 0');
          }
          try {
            c.sink({ sink: EastWood.AudioSink_None });
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('sink');
            expect(e.toString()).to.contain('Needs 2');
            expect(e.toString()).to.contain('given 1');
          }
          try {
            c.sink({}, { sink: EastWood.VideoSink_File });
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('sink');
            expect(e.toString()).to.contain('audio sink type');
          }
          try {
            c.sink({ sink: EastWood.AudioSink_None }, { sink: EastWood.VideoSink_File });
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('sink');
            expect(e.toString()).to.contain('video sink filename');
          }
        });
        it('should throw if given more args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.sink({ sink: EastWood.AudioSink_None }, { sink: EastWood.VideoSink_None }, 123);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('sink');
            expect(e.toString()).to.contain('Takes 2');
            expect(e.toString()).to.contain('given 3');
          }
        });
        it('should throw if given incorrect args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.sink('aaa', {});
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('sink');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('given aaa');
          }
          try {
            c.sink({ sink: 999 }, {});
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('sink');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('audio sink type 999');
          }
          try {
            c.sink({ sink: EastWood.AudioSink_File, filename: 123 }, { sink: EastWood.VideoSink_None });
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('sink');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('audio sink filename 123');
          }
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          c = ew.createSubscriber().configuration()
                        .sink({ sink: EastWood.AudioSink_None }, { sink: EastWood.VideoSink_None })
                        .toObject();
          expect(c.audio.sink).to.equal('none');
          expect(c.video.sink).to.equal('none');

          c = ew.createSubscriber().configuration()
                        .sink({ sink: EastWood.AudioSink_File, filename: 'file/name.a' },
                              { sink: EastWood.VideoSink_File, filename: 'file/name.b' })
                        .toObject();
          expect(c.audio.sink).to.equal('file');
          expect(c.audio.filename).to.equal('file/name.a');
          expect(c.video.sink).to.equal('file');
          expect(c.video.filename).to.equal('file/name.b');
        });
      });

      describe('ffmpegSink', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.ffmpegSink();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('ffmpegSink');
            expect(e.toString()).to.contain('Needs 2');
            expect(e.toString()).to.contain('given 0');
          }
          try {
            c.ffmpegSink("abc");
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('ffmpegSink');
            expect(e.toString()).to.contain('Needs 2');
            expect(e.toString()).to.contain('given 1');
          }
        });
        it('should throw if given more args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.ffmpegSink('a.mp4', 'some params', 'and-extra');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('ffmpegSink');
            expect(e.toString()).to.contain('Takes 2');
            expect(e.toString()).to.contain('given 3');
          }
        });
        it('should throw if given incorrect args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.ffmpegSink(123, 'aaa');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('ffmpegSink');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('given 123');
          }
          try {
            c.ffmpegSink('a.mp4', 11);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('ffmpegSink');
            expect(e.toString()).to.contain('Wrong argument at 1');
            expect(e.toString()).to.contain('given 11');
          }
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          c = ew.createSubscriber().configuration()
                        .ffmpegSink('some/where/abc.mp4', '-param1=1 -param2=2')
                        .toObject();
          expect(c.ffmpeg.output).to.equal('some/where/abc.mp4');
          expect(c.ffmpeg.params).to.equal('-param1=1 -param2=2');
        });
      });

      describe('subscriptionErrorRetry', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.subscriptionErrorRetry();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('subscriptionErrorRetry');
            expect(e.toString()).to.contain('Needs 3');
            expect(e.toString()).to.contain('given 0');
          }
          try {
            c.subscriptionErrorRetry(1);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('subscriptionErrorRetry');
            expect(e.toString()).to.contain('Needs 3');
            expect(e.toString()).to.contain('given 1');
          }
          try {
            c.subscriptionErrorRetry(1, 2);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('subscriptionErrorRetry');
            expect(e.toString()).to.contain('Needs 3');
            expect(e.toString()).to.contain('given 2');
          }
        });
        it('should throw if given incorrect args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration();
          try {
            c.subscriptionErrorRetry('aaa', 2, 3);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('subscriptionErrorRetry');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('given aaa');
          }
          try {
            c.subscriptionErrorRetry(1, 'aaa', 3);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('subscriptionErrorRetry');
            expect(e.toString()).to.contain('Wrong argument at 1');
            expect(e.toString()).to.contain('given aaa');
          }
          try {
            c.subscriptionErrorRetry(1, 2, 'aaa');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('subscriptionErrorRetry');
            expect(e.toString()).to.contain('Wrong argument at 2');
            expect(e.toString()).to.contain('given aaa');
          }
          try {
            c.subscriptionErrorRetry(1, 2, 0.9);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('subscriptionErrorRetry');
            expect(e.toString()).to.contain('Wrong argument at 2');
            expect(e.toString()).to.contain('retry delay progression must be 1.0 or bigger');
          }
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          c = ew.createSubscriber().configuration()
                        .subscriptionErrorRetry(1, 2, 3)
                        .toObject();
          expect(c.retry.max).to.equal(1);
          expect(c.retry.initDelay_ms).to.equal(2);
          expect(c.retry.progression).to.equal(3);
        });
      });

      describe('Configuration integrity', function() {
        it('should throw if none of bixby and allocator were given', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration()
            // set other mandatory items
            .streamNotifier('host', 123, 'tag', true, true).userId('uuu').duration('infinite')
            .sink({ sink: EastWood.AudioSink_None }, { sink: EastWood.VideoSink_None });
          try {
            c.verify();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('Need either Bixby endpoint or Allocator endpoint');
          }
        });
        it('should throw if both bixby and allocator were given', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration()
            // set other mandatory items
            .streamNotifier('host', 123, 'tag', true, true).userId('uuu').duration('infinite')
            .sink({ sink: EastWood.AudioSink_None }, { sink: EastWood.VideoSink_None })
            // and conflicting items
            .bixby('host1', 10)
            .bixbyAllocator('host2', 20, 'locA');
          try {
            c.verify();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('Bixby endpoint and Allocator endpoint are mutually exclusive');
          }
        });
        it('should throw if none of notifier and stream url were given', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration()
            // set other mandatory items
            .bixby('host', 123).userId('uuu').duration('infinite')
            .sink({ sink: EastWood.AudioSink_None }, { sink: EastWood.VideoSink_None });
          try {
            c.verify();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('Need either Stream notifier endpoint or Stream URL');
          }
        });
        it('should throw if both bixby and allocator were given', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration()
            // set other mandatory items
            .bixby('host1', 10).userId('uuu').duration('infinite')
            .sink({ sink: EastWood.AudioSink_None }, { sink: EastWood.VideoSink_None })
            // and conflicting items
            .streamNotifier('host2', 123, 'tag', true, true)
            .streamUrl('surl2');
          try {
            c.verify();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('Stream notifier endpoint and Stream URL are mutually exclusive');
          }
        });
        it('should throw if duration was not given', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration()
            // set other mandatory items
            .bixby('host1', 10).streamUrl('surl2').userId('uuu')
            .sink({ sink: EastWood.AudioSink_None }, { sink: EastWood.VideoSink_None });
          try {
            c.verify();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('Need duration');
          }
        });
        it('should throw if user id was not given', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration()
            // set other mandatory items
            .bixby('host1', 10).streamUrl('surl2').duration('infinite')
            .sink({ sink: EastWood.AudioSink_None }, { sink: EastWood.VideoSink_None });
          try {
            c.verify();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('Need user id');
          }
        });
        it('should throw if no sink was given', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration()
            // set other mandatory items
            .bixby('host1', 10).streamUrl('surl2').duration('infinite').userId('aa');
          try {
            c.verify();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('Need sink');
          }
        });
        it('should throw if regular sink and ffmpeg sink were both given', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configuration()
            // set other mandatory items
            .bixby('host1', 10).streamUrl('surl2').duration('infinite').userId('aa')
            // then conflicts
            .sink({ sink: EastWood.AudioSink_None }, { sink: EastWood.VideoSink_None })
            .ffmpegSink('z.mp4', '--some-param=123');
          try {
            c.verify();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('Regular sink and FFMpeg sink are mutually exclusive');
          }
        });
      });
    });

    // describe('Subscriber', function() {
    //   it('starts', function(done) {
    //     this.timeout(20000);
    //     const ew = new EastWood(EastWood.LogLevel_Debug, true, false);  // console-log, no-syslog
    //     const sub = ew.createSubscriber()
    //     sub.configuration()
    //       .bixbyAllocator('media-allocator.eng.airtime.com', 8192, 'at-west')
    //       .userId('art123')
    //       .streamNotifier('stream-notifier.eng.signal.is', 443, 'arttest', true, true)  // TLS, cert
    //       .authSecret('hello@test-eng')
    //       .sink({ sink: EastWood.AudioSink_None }, { sink: EastWood.VideoSink_None })
    //       .duration('00:00:10');
    //     console.log(sub.configuration().toObject());

    //     sub.on("finish", function(err) {
    //       console.log("Finish: " + err);
    //       sub.stop();
    //       done();
    //     });

    //     sub.start();
    //   });
    // });

  });
});
