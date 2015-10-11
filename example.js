var eventLoopStats = require('.');
var fs = require('fs');

setInterval(function(){
  console.log('New Stats', eventLoopStats.sense());
}, 1000);


// put mild stress on the event loop
setInterval(function() {
  fs.readFile(__dirname + '/LICENSE', function() {
    var foo = [];
    for (var i = 0; i < 1000000; i++) {
      foo.push(new Date());
    }
  });
}, 5000);
