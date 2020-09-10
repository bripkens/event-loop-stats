interface Stats {
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

/**
 * Exposes function to get stats about the libuv default loop
 */
declare const EventLoopStats: {
    /**
     * Exposes stats about the libuv default loop
     */
    sense: () => Stats;
};

export = EventLoopStats;