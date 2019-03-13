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

  it('should detect blocking operations', function(done) {
    const DELAY = 100;

    setTimeout(function() {
      // block the event loop
      const end = Date.now() + DELAY;
      while (Date.now() < end) {}
    });

    setTimeout(function() {
      var stats = eventLoopStats.sense();
      expect(stats.max).to.be.gte(DELAY);
      expect(stats.sum).to.be.gte(stats.min + stats.max);
      done();
    }, 200);
  });

});
