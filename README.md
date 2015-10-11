<h1 align="center">event-loop-stats</h1>
<p align="center">Exposes stats about the libuv default loop</p>

[![Build Status](https://travis-ci.org/bripkens/event-loop-stats.svg?branch=master)](https://travis-ci.org/bripkens/event-loop-stats)

## Installation

```
npm install --save event-loop-stats
```

## Usage
```javascript
var eventLoopStats = require('event-loop-stats');
console.log('Stats', eventLoopStats.sense());
```

This will print the following information:

```
Stats {
  max: 5,
  min: 0,
  sum: 10,
  num: 5
}
```

## Property insights
 - `max`: Maximum number of milliseconds spent in a single loop since last `sense call`.
 - `min`: Minimum number of milliseconds spent in a single loop since last `sense call`.
 - `sum`: Total number of milliseconds spent in the loop since last `sense call`.
 - `num`: Total number of loops since last `sense call`.

## Node version compatibility
`event-loop-stats` depends on C++ extensions which are compiled when the `event-loop-stats` module is installed. Compatibility information can be inspected via the [Travis-CI build jobs](https://travis-ci.org/bripkens/event-loop-stats).
