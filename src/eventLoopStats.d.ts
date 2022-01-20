export interface EventLoopStats {
  /**
   * Maximum number of milliseconds spent in a single loop since last sense call
   */
  max: number;
  /**
   * Minimum number of milliseconds spent in a single loop since last sense call
   */
  min: number;
  /**
   * Total number of milliseconds spent in the loop since last sense call
   */
  sum: number;
  /**
   * Total number of loops since last sense call
   */
  num: number;
}

export function sense(): EventLoopStats;

