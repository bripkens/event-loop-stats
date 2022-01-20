import {sense} from '../src/eventLoopStats';

const stats = sense();

const max: number = stats.max;
const min: number = stats.min;
const sum: number = stats.sum;
const num: number = stats.num;

console.log({
  max,
  min,
  sum,
  num
});
