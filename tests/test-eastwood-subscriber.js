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

  it('should throw if not created by new', function() {
    try {
      var e = EastWood(EastWood.LogLevel_Info, true, true);
      expect(false).to.be.ok;
    } catch (e) {
      expect(e.toString()).to.contain("Use 'new' to instantiate");
    }
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
    expect(EastWood.AudioSink_FFMpeg).to.be.a('number');
    expect(EastWood.AudioSink_None).to.not.equal(EastWood.AudioSink_File);
    expect(EastWood.AudioSink_None).to.not.equal(EastWood.AudioSink_FFMpeg);
    expect(EastWood.AudioSink_File).to.not.equal(EastWood.AudioSink_FFMpeg);

    expect(EastWood.VideoSink_None).to.be.a('number');
    expect(EastWood.VideoSink_File).to.be.a('number');
    expect(EastWood.VideoSink_FFMpeg).to.be.a('number');
    expect(EastWood.VideoSink_None).to.not.equal(EastWood.VideoSink_File);
    expect(EastWood.VideoSink_None).to.not.equal(EastWood.VideoSink_FFMpeg);
    expect(EastWood.VideoSink_File).to.not.equal(EastWood.VideoSink_FFMpeg);
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
    describe('Configure', function() {
      describe('bixby', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure();
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
            c.bixby('host', 'abc');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('bixby');
            expect(e.toString()).to.contain('Wrong argument at 1');
            expect(e.toString()).to.contain('given abc');
          }
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configure()
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
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure();
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
            c.bixbyAllocator('host', 'abc', 'lll');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('bixbyAllocator');
            expect(e.toString()).to.contain('Wrong argument at 1');
            expect(e.toString()).to.contain('given abc');
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
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configure()
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
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure();
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
            c.streamNotifier('host', 'abc', 'lll', false, true);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamNotifier');
            expect(e.toString()).to.contain('Wrong argument at 1');
            expect(e.toString()).to.contain('given abc');
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
          const c = ew.createSubscriber().configure()
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
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure();
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
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configure()
                        .duration('00:01:23')
                        .toObject();
          expect(c.duration_ms).to.equal(83000);
        });
      });

      describe('userId', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure();
          try {
            c.userId(123);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('userId');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('given 123');
          }
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configure()
                        .userId('user123')
                        .toObject();
          expect(c.userId).to.equal('user123');
        });
      });

      describe('streamUrl', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure();
          try {
            c.streamUrl(123);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('streamUrl');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('given 123');
          }
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configure()
                        .streamUrl('s123')
                        .toObject();
          expect(c.streamURL).to.equal('s123');
        });
      });

      describe('certCheck', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure();
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
          c = ew.createSubscriber().configure()
                        .certCheck(true)
                        .toObject();
          expect(c.cert).to.equal(true);
          c = ew.createSubscriber().configure()
                        .certCheck(false)
                        .toObject();
          expect(c.cert).to.equal(false);
        });
      });

      describe('authSecret', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure()
                        .authSecret('sec123')
                        .toObject();
          expect(c.secret).to.equal('sec123');
        });
      });

      describe('printFrameInfo', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure();
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
          c = ew.createSubscriber().configure()
                        .printFrameInfo(true)
                        .toObject();
          expect(c.frameInfo).to.equal(true);
          c = ew.createSubscriber().configure()
                        .printFrameInfo(false)
                        .toObject();
          expect(c.frameInfo).to.equal(false);
        });
      });

      describe('audioSink', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configure();
          try {
            c.audioSink();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('audioSink');
            expect(e.toString()).to.contain('Needs 1');
            expect(e.toString()).to.contain('given 0');
          }
          try {
            c.audioSink(EastWood.AudioSink_File);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('audioSink');
            expect(e.toString()).to.contain('Need sink param');
          }
          try {
            c.audioSink(EastWood.AudioSink_FFMpeg);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('audioSink');
            expect(e.toString()).to.contain('Need sink param');
          }
        });
        it('should throw if given more args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configure();
          try {
            c.audioSink(EastWood.AudioSink_None, 'file123');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('audioSink');
            expect(e.toString()).to.contain('Sink param should not be given');
          }
          try {
            c.audioSink(EastWood.AudioSink_File, 'file/name', 123);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('audioSink');
            expect(e.toString()).to.contain('Takes 2');
            expect(e.toString()).to.contain('given 3');
          }
          try {
            c.audioSink(EastWood.AudioSink_FFMpeg, 'filename', 123);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('audioSink');
            expect(e.toString()).to.contain('Takes 2');
            expect(e.toString()).to.contain('given 3');
          }
        });
        it('should throw if given incorrect args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configure();
          try {
            c.audioSink('aaa');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('audioSink');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('given aaa');
          }
          try {
            c.audioSink(999);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('audioSink');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('given 999');
          }
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          c = ew.createSubscriber().configure()
                        .audioSink(EastWood.AudioSink_None)
                        .toObject();
          expect(c.audio.sink).to.equal('none');

          c = ew.createSubscriber().configure()
                        .audioSink(EastWood.AudioSink_File, 'file/name.a')
                        .toObject();
          expect(c.audio.sink).to.equal('file');
          expect(c.audio.param).to.equal('file/name.a');

          c = ew.createSubscriber().configure()
                        .audioSink(EastWood.AudioSink_FFMpeg, 'file/name.mp4')
                        .toObject();
          expect(c.audio.sink).to.equal('ffmpeg');
          expect(c.audio.param).to.equal('file/name.mp4');
        });
      });

      describe('videoSink', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configure();
          try {
            c.videoSink();
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('videoSink');
            expect(e.toString()).to.contain('Needs 1');
            expect(e.toString()).to.contain('given 0');
          }
          try {
            c.videoSink(EastWood.VideoSink_File);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('videoSink');
            expect(e.toString()).to.contain('Need sink param');
          }
          try {
            c.videoSink(EastWood.VideoSink_FFMpeg);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('videoSink');
            expect(e.toString()).to.contain('Need sink param');
          }
        });
        it('should throw if given more args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configure();
          try {
            c.videoSink(EastWood.VideoSink_None, 'file123');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('videoSink');
            expect(e.toString()).to.contain('Sink param should not be given');
          }
          try {
            c.videoSink(EastWood.VideoSink_File, 'file/name', 123);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('videoSink');
            expect(e.toString()).to.contain('Takes 2');
            expect(e.toString()).to.contain('given 3');
          }
          try {
            c.videoSink(EastWood.VideoSink_FFMpeg, 'filename', 123);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('videoSink');
            expect(e.toString()).to.contain('Takes 2');
            expect(e.toString()).to.contain('given 3');
          }
        });
        it('should throw if given incorrect args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configure();
          try {
            c.videoSink('aaa');
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('videoSink');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('given aaa');
          }
          try {
            c.videoSink(11);
            expect(false).to.be.ok;
          } catch (e) {
            expect(e.toString()).to.contain('SubscriberConfig');
            expect(e.toString()).to.contain('videoSink');
            expect(e.toString()).to.contain('Wrong argument at 0');
            expect(e.toString()).to.contain('given 11');
          }
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          c = ew.createSubscriber().configure()
                        .videoSink(EastWood.VideoSink_None)
                        .toObject();
          expect(c.video.sink).to.equal('none');

          c = ew.createSubscriber().configure()
                        .videoSink(EastWood.VideoSink_File, 'file/name.a')
                        .toObject();
          expect(c.video.sink).to.equal('file');
          expect(c.video.param).to.equal('file/name.a');

          c = ew.createSubscriber().configure()
                        .videoSink(EastWood.VideoSink_FFMpeg, 'file/name.mp4')
                        .toObject();
          expect(c.video.sink).to.equal('ffmpeg');
          expect(c.video.param).to.equal('file/name.mp4');
        });
      });

      describe('subscriptionErrorRetry', function() {
        it('should throw if given insufficient args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          const c = ew.createSubscriber().configure();
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
          const c = ew.createSubscriber().configure();
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
        });
        it('should set if given correct args', function() {
          const ew = new EastWood(testLogLevel, true, false);
          c = ew.createSubscriber().configure()
                        .subscriptionErrorRetry(1, 2, 3)
                        .toObject();
          expect(c.retry.max).to.equal(1);
          expect(c.retry.initDelay_ms).to.equal(2);
          expect(c.retry.progression).to.equal(3);
        });
      });
    });
  });
});
