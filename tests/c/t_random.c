/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Tests for pseudo-random number generator.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <math.h>

#include "ojcardlib.h"
#include "stats.h"

/* Quickly verify that giving a non-zero seed gives us a repeatable sequence,
 * and that a zero seed doesn't.
 */

int allequal(int *a, int *b, int n) {
    int i;
    for (i = 0; i < n; ++i) { if (a[i] != b[i]) return 0; }
    return 1;
}

int seed_test(void) {
    int i, j;
    int seeds[] = { 0x12345678, 0x9ABCDEF0, 0, 0x12345678, 0x9ABCDEF0, 0 };
    uint32_t vals[6][100];

    for (i = 0; i < 6; ++i) {
        ojr_seed(seeds[i]);
        for (j = 0; j < 100; ++j) { vals[i][j] = ojr_next32(); }
    }
    if (! allequal(vals[0], vals[3], 100)) return 10;
    if (! allequal(vals[1], vals[4], 100)) return 20;
    for (i = 0; i < 10; ++i) {
        if (allequal(vals[2] + 10 * i, vals[5] + 10 * i, 10)) return 30;
    }
    return 0;
}

/* Test that ojr_rand() gives a balanced random number within limits. Fill
 * buckets with a few million of them and measure the variance from the
 * expected value.
 */
int balance_test(int count) {
    int i, j, v, n, bcount[] = { 7, 31, 52, 53, };
    struct buckets *bp;
    assert(count > 1);

    fprintf(stderr, "Balance test count = %d...\n", count);
    for (i = 0; i < 4; ++i) {
        n = bcount[i];
        bp = create_buckets(n);

        for (j = 0; j < count; ++j) {
            v = ojr_rand(n);
            add_value(bp, v);
        }
        calculate_stats(bp);
        fprintf(stderr, "%3d buckets: mean = %10.2f, stddev = %7.2f (%4.2f %%), maxz = %4.2f\n",
            n, bp->mean, bp->stddev, (100.0 * bp->stddev) / bp->mean, bp->maxz);

        if ((bp->stddev / bp->mean) > 0.005) return 1;
        if (bp->maxz > 4.0) return 2;
        free_buckets(bp);
    }
    return 0;
}

/* Myrvold and Ruskey linear-time permutation rank algorithm.
 */
#define SWAP(a,b) do{t=(a);(a)=(b);(b)=t;}while(0)

int _mr_rank1(int n, int *vec, int *inv) {
    int s, t;
    if (n < 2) return 0;

    s = vec[n-1];
    SWAP(vec[n-1], vec[inv[n-1]]);
    SWAP(inv[s], inv[n-1]);
    return s + n * _mr_rank1(n-1, vec, inv);
}

int get_rank(oj_cardlist_t *sp) {
    int i, vec[10], inv[10], n = sp->length;
    assert(n <= 10);

    for (i = 0; i < n; ++i) {
        vec[i] = OJL_GET(sp, i) - 1;
        inv[OJL_GET(sp, i) - 1] = i;
    }
    return _mr_rank1(n, vec, inv);
}

int shuffle_test(int count) {
    int i, j, r, h, b, dbuf[54], hbuf[20];
    char cbuf[64];
    oj_cardlist_t deck, hand;
    struct buckets *bp[5];
    oj_combiner_t comb;

    ojl_new(&deck, dbuf, 52);
    ojl_new(&hand, hbuf, 20);
    fprintf(stderr, "Shuffle test count = %d...\n", count);

    ojl_fill(&hand, 4, OJD_STANDARD);
    bp[0] = create_buckets(24);

    for (i = 0; i < count; ++i) {
        ojl_shuffle(&hand);
        r = get_rank(&hand);
        add_value(bp[0], r);
    }
    calculate_stats(bp[0]);
    fprintf(stderr, "%3d buckets: mean = %10.2f, stddev = %7.2f (%4.2f %%), maxz = %4.2f\n",
        24, bp[0]->mean, bp[0]->stddev, (100.0 * bp[0]->stddev) / bp[0]->mean, bp[0]->maxz);

    if ((bp[0]->stddev / bp[0]->mean) > 0.005) return 1;
    if (bp[0]->maxz > 4.0) return 2;
    free_buckets(bp[0]);

    for (b = 0; b < 5; ++b) bp[b] = create_buckets(52);
    ojl_fill(&deck, 52, OJD_STANDARD);

    for (i = 0; i < count; ++i) {
        ojl_fill(&deck, 52, OJD_STANDARD);
        ojl_shuffle(&deck);

        add_value(bp[0], OJL_GET(&deck, 0));
        add_value(bp[1], OJL_GET(&deck, 13));
        add_value(bp[2], OJL_GET(&deck, 26));
        add_value(bp[3], OJL_GET(&deck, 38));
        add_value(bp[4], OJL_GET(&deck, 51));

        if (0 == (i & 0x3FFFF)) {
            fprintf(stderr, "\r%10d\r", count - i);
            fflush(stderr);
        }
    }
    for (b = 0; b < 5; ++b) {
        calculate_stats(bp[b]);
        fprintf(stderr, "%3d buckets: mean = %10.2f, stddev = %7.2f (%4.2f %%), maxz = %4.2f\n",
            52, bp[b]->mean, bp[b]->stddev, (100.0 * bp[b]->stddev) / bp[b]->mean, bp[b]->maxz);

        if ((bp[b]->stddev / bp[b]->mean) > 0.005) return 3;
        if (bp[b]->maxz > 4.0) return 4;
        free_buckets(bp[b]);
    }
    return 0;
}

int montecarlo_test(int count) {
    int i, j, r, dbuf[20], hbuf[20];
    char cbuf[64];
    oj_cardlist_t deck, hand;
    struct buckets *bp;
    oj_combiner_t comb;

    ojl_new(&deck, dbuf, 20);
    ojl_new(&hand, hbuf, 20);
    fprintf(stderr, "Monte Carlo test count = %d...\n", count);

    ojl_fill(&deck, 6, OJD_STANDARD);
    ojc_new(&comb, &deck, &hand, 3, (long long)count);

    assert(20LL == comb.total);
    bp = create_buckets(20);

    while (ojc_next_random(&comb)) {
        r = (int)ojc_colex_rank(&hand, &comb);
        add_value(bp, r);
    }
    calculate_stats(bp);
    fprintf(stderr, "%3d buckets: mean = %10.2f, stddev = %7.2f (%4.2f %%), maxz = %4.2f\n",
        20, bp->mean, bp->stddev, (100.0 * bp->stddev) / bp->mean, bp->maxz);

    if ((bp->stddev / bp->mean) > 0.005) return 1;
    if (bp->maxz > 4.0) return 2;
    free_buckets(bp);
    return 0;
}

/* This function never returns! It continuously feeds random bits to stdout,
 * which can be piped into "dieharder -a -g 200" to verify the PRNG.
 */
void feed_dieharder(void) {
    int i;
    uint32_t buf[100];

    while (1) {
        for (i = 0; i < 100; ++i) {
            buf[i] = ojr_next32();
        }
        write(STDOUT_FILENO, buf, sizeof(buf));
    }
}

int main(int argc, char *argv[]) {
    int r, failed = 0;

    failed |= (r = seed_test());
    fprintf(stderr, "Seed test %sed.\n", (r ? "fail" : "pass"));

    failed |= (r = balance_test(10000000));
    fprintf(stderr, "Balance test %sed.\n", (r ? "fail" : "pass"));

    failed |= (r = shuffle_test(10000000));
    fprintf(stderr, "Shuffle test %sed.\n", (r ? "fail" : "pass"));

    failed |= (r = montecarlo_test(10000000));
    fprintf(stderr, "Monte Carlo test %sed.\n", (r ? "fail" : "pass"));

    if ((argc > 1) && (0 == strcmp("-d", argv[1]))) {
        fprintf(stderr, "Feeding dieharder tests...\n");
        feed_dieharder();
        /* Never exits */
    }
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
