var eventLoopStats = require('..');
var expect = require('chai').expect;

describe('eventLoopStats', function() {

  it('should expose a sense function', function() {
    expect(eventLoopStats.sense).to.be.a('function');
  });

  it('should return stats via the sense function', function() {
    var stats = eventLoopStats.sense();
    expect(stats.num).to.be.a('number');
    expect(stats.sum).to.be.a('number');
    expect(stats.max).to.be.a('number');
    expect(stats.min).to.be.a('number');
  });

});
