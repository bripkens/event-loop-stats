var eventLoopStats = require('..');
var expect = require('chai').expect;

describe('eventLoopStats', function() {
  it('should expose a sense function', function() {
    expect(eventLoopStats.sense).to.be.a('function');
  });

  it('should return stats via the sense function', function(done) {
    setTimeout(function() {
      // provoke execution on the default loop
    }, 500);

    setTimeout(function() {
      var stats = eventLoopStats.sense();
      expect(stats.num).to.be.a('number');
      expect(stats.num).to.be.gt(0);
      expect(stats.sum).to.be.a('number');
      expect(stats.max).to.be.a('number');
      expect(stats.min).to.be.a('number');
      done();
    }, 1000);
  });

  // This pattern will usually allow for at least _two_ on_check calls between
  // the two sense calls. Check the next test for a slightly different pattern.
  it('should detect a blocked event loop', function(done) {
    // Call sense to reset max.
    eventLoopStats.sense();
    // On the next tick, block for 500ms.
    setTimeout(function() {
      var waitUntill = new Date(Date.now() + 500);
      // Block event loop with busy waiting.
      while (waitUntill > new Date()) {}

      // On the next tick, detect stats again - the 500 ms block should have
      // been noticed.
      setTimeout(function() {
        var stats = eventLoopStats.sense();
        expect(stats.max).to.be.gte(490);
        expect(stats.max).to.be.lt(2000);
        expect(stats.sum).to.be.gte(490);
        expect(stats.sum).to.be.lt(2000);

        // At least two on_check calls should have happened (older Node.js versions will call it more often).
        expect(stats.num).to.be.gte(2);

        // Since there are at least two on_check calls, min and max should be different.
        expect(stats.min).to.be.gte(0);
        expect(stats.min).to.be.lt(stats.max);

        done();
      }, 0);
    }, 0);
  });

  // This pattern will typically allow for only _one_ on_check call between the
  // two sense calls (at least on more recent Node.js versions). Check the
  // previous test for a slightly different pattern.
  it('one on_check call should suffice to report correct max duration ', function(done) {
    // Call sense to reset max.
    eventLoopStats.sense();
    var now = Date.now();
    var end = now + 50;
    setTimeout(function() {
      // After 100ms, call sense again - the 50ms block (see below) should have been noticed.
      var stats = eventLoopStats.sense();
      expect(stats.max).to.be.gte(40);
      expect(stats.max).to.be.lt(1000);
      expect(stats.sum).to.be.gte(40);
      expect(stats.sum).to.be.lt(1000);

      // Only one on_check call might have happened.
      expect(stats.num).to.gte(1);

      done();
    }, 100);

    // Directly block for 50ms right in this tick, synchronously.
    while (Date.now() < end) {}
  });
});
