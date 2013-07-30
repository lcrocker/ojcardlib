/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

#include "ojcardlib.h"
#include "stats.h"

oj_cardlist deck, hand1, hand2;
oj_combiner iter1, iter2;
oj_card dbuf[54], hbuf1[16], hbuf2[16];

void initialize(void) {
    ojl_new(&deck, dbuf, 53);
    ojl_fill(&deck, 52, OJD_STANDARD);
    ojl_new(&hand1, hbuf1, 16);
    ojl_new(&hand2, hbuf2, 16);
}

int validate(void) {
    int i, f[54];

    for (i = 0; i < iter1.deck->length; ++i) {
        f[i] = 0;
        if (i != iter1.invert[iter1.deck->cards[i]]) return 2;
    }
    for (i = 0; i < iter1.k; ++i) {
        if (iter1.map[i] < 0 || iter1.map[i] >= iter1.deck->length) return 1;
        if (++f[iter1.map[i]] > 1) return 3;
    }
    return 0;
}

int nvals[] = { 52, 50, 40, 31, 22, 16, 12, 7 };
int kvals[] = {  2,  3,  4,  5,  6,  7,  7, 7 };
long long tvals[] = { 1326, 19600, 91390, 169991, 74613, 11440, 792, 1 };

int test_combinations(int n, int k) {
    int i, r;
    int64_t t, c;
    assert(0x10ACE0FF == deck._johnnymoss);

    ojl_fill(&deck, 52, OJD_STANDARD);
    ojl_shuffle(&deck);
    ojl_truncate(&deck, n);
    ojc_new(&iter1, &deck, &hand1, k, 0LL);

    c = 0;
    while (ojc_next(&iter1)) {
        if (c != (iter1.total - iter1.remaining) - 1) return 10;

        t = ojc_colex_rank(&iter1, &hand1);
        ojc_colex_hand_at(&iter1, t, &hand2);
        if (! ojl_equal(&hand2, &hand1)) return 30;
        ++c;

        if (0 == ojr_rand(10000)) {
            r = validate();
            if (r) return r + 20;
        }
    }
    for (i = 0; i < k; ++i) {
        hand2.cards[i] = deck.cards[deck.length - k + i];
        hand2.length = k;
    }
    if (! ojl_equal(&hand1, &hand2)) return 40;
    return 0;
}

int loop_combinations(void) {
    int i, k, n, r;

    for (i = 0; i < 8; ++i) {
        n = nvals[i];
        k = kvals[i];

        r = test_combinations(n, k);
        if (0 != r) return r;

        n = 20 + ojr_rand(10);
        k = 1 + ojr_rand(4);

        r = test_combinations(n, k);
        if (0 != r) return r;
    }
    return 0;
}

int test_montecarlo(int n, int k, long long count) {
    int r;
    int64_t t;
    assert(0x10ACE0FF == deck._johnnymoss);

    ojl_fill(&deck, 52, OJD_STANDARD);
    ojl_shuffle(&deck);
    ojl_truncate(&deck, n);

    t = ojc_new(&iter1, &deck, &hand1, k, count);
    while (ojc_next_random(&iter1)) {
        t = ojc_colex_rank(&iter1, &hand1);
        ojc_colex_hand_at(&iter1, t, &hand2);

        if (0 == ojr_rand(10000)) {
            r = validate();
            if (r) return r + 60;
        }
    }
    return 0;
}

int loop_montecarlo(void) {
    int i, k, n, r;

    for (i = 0; i < 8; ++i) {
        n = 20 + ojr_rand(10);
        k = 2 + ojr_rand(4);

        r = test_montecarlo(n, k, 100000LL);
        if (0 != r) return r;
    }
    return 0;
}

int random_balance(void) {
    int64_t count = 10000000;
    struct counter *bp;

    ojl_fill(&deck, 52, OJD_STANDARD);
    ojc_new(&iter1, &deck, &hand1, 5, count);
    bp = create_counter(52);

    fprintf(stderr, "Combiner balance test count = %lld...\n", count);
    while (ojc_next_random(&iter1)) {
        for (int i = 0; i < 5; ++i) INC(bp, hand1.cards[i] - 1);
    }
    counter_report(bp);
    free_counter(bp);
    return 0;
}

int main(int argc, char *argv[]) {
    int r, failed = 0;

    initialize();
    r = loop_combinations();
    failed |= r;
    fprintf(stderr, "Combinations test %sed.\n", (r ? "fail" : "pass"));

    r = loop_montecarlo();
    failed |= r;
    fprintf(stderr, "Monte carlo test %sed.\n", (r ? "fail" : "pass"));

    r = random_balance();
    failed |= r;
    fprintf(stderr, "Balance test %sed.\n", (r ? "fail" : "pass"));

    fprintf(stderr, "Combinatorics tests ");
    if (failed) {
        fprintf(stderr, "failed. Code: %d\n", failed);
    } else {
        fprintf(stderr, "passed.\n");
    }
    (void)(argc);
    (void)(argv); // keep -Wextra happy
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
