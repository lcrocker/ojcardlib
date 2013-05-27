/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Statistics routines for various test programs.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

struct buckets {
    double mean, stddev, maxz;
    long n, count;
    long buckets[1];
};

struct buckets *create_buckets(int n) {
    struct buckets *bp;
    int i;

    bp = malloc(((n - 1) * sizeof(long)) + sizeof(struct buckets));
    bp->mean = bp->stddev = bp->maxz = 0.0;
    bp->count = 0L;
    bp->n = n;
    for (i = 0; i < n; ++i) { bp->buckets[i] = 0L; }
    return bp;
}
void free_buckets(struct buckets *bp) { free(bp); }

int add_value(struct buckets *bp, int v) {
    ++bp->count;
    ++bp->buckets[v % bp->n];
    return 0;
}

int calculate_stats(struct buckets *bp) {
    int i;
    double d, z, t = 0.0;
    bp->mean = (double)(bp->count) / (double)(bp->n);

    for (i = 0; i < bp->n; ++i) {
        d = (double)(bp->buckets[i]) - bp->mean;
        t += d * d;
    }
    bp->stddev = sqrt( t / (double)(bp->n - 1) );
    bp->maxz = 0.0;
    for (i = 0; i < bp->n; ++i) {
        z = fabs(((double)(bp->buckets[i]) - bp->mean) / bp->stddev);
        if (z > bp->maxz) bp->maxz = z;
    }
}
