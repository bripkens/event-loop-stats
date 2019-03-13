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

  it('should actually detect a blocked eventloop', function(done) {
    // Get stats to reset max
    eventLoopStats.sense();
    // On the next tick, block for 500ms
    setTimeout(function() {
      var waitUntill = new Date(Date.now() + 500);
      while(waitUntill > new Date()) {};

      // On the next tick, detect stats again
      setTimeout(function() {
        var stats = eventLoopStats.sense();
        expect(stats.max).to.be.gt(450);
        done();
      }, 0);
    }, 0)
  });

});
