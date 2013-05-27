/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Tracking combinations of <k> cards from a larger set.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

#include "ojcardlib.h"
#include "bctable.h"

/* Return (n choose k).
 */
__attribute__((hot, pure))
long long ojc_binomial(int n, int k) {
    int i;
    long long b;
    assert(n >= 0 && k >= 0);

    if (0 == k || n == k) return 1LL;
    if (k > n) return 0LL;

    if (k > (n - k)) k = n - k;
    if (1 == k) return (long long)n;

    if (n <= 54 && k <= 54) {
        return bctable[(((n - 3) * (n - 3)) >> 2) + k];
    }
    /* Last resort: actually calculate */
    b = 1LL;
    for (i = 1; i <= k; ++i) {
        b *= (n - (k - i));
        if (b < 0) return -1LL; /* Overflow */
        b /= i;
    }
    return b;
}

/* Initialize a new combiner. We need to provide a <deck> sequence that is
 * the set to make combinations of, a mutable <hand> sequence to receive the
 * combinations, <k> for the size of the combinations, and a <k>-sized integer
 * buffer for internal use. If <count> is nonzero, the monte carlo interator
 * will stop after that many iterations.
 */
int ojc_new(
    oj_combiner_t *comb,        /* Combiner to initialize */
    oj_cardlist_t *deck,        /* Universe set of cards */
    oj_cardlist_t *hand,        /* Hand to put results into */
    int k,                      /* Cards per hand */
    long long count)            /* 0 for all combos, >0 for random count */
{
    int i;

    assert(0 != comb && 0 != deck && 0 != hand && 0 != k);
    assert(hand->allocation >= k);
    assert(deck->length >= k && deck->length <= 54);

    if (hand->pflags & OJF_RDONLY) return OJE_RDONLY;

    comb->_johnnymoss = 0x10ACE0FF;
    comb->deck = deck;
    comb->hand = hand;
    comb->k = k;

    comb->total = ojc_binomial(deck->length, k);
    comb->rank = 0LL;
    if (0 == count) comb->remaining = comb->total;
    else comb->remaining = count;

    hand->length = k;
    for (i = 0; i < k; ++i) hand->cards[i] = deck->cards[i];
    for (i = 0; i < deck->length; ++i) {
        comb->map[i] = comb->deck_invert[deck->cards[i]] = i;
    }
    comb->flags = OJF_VALIDMAP | OJF_VALIDRANK;

    hand->eflags = 0;
    comb->extra = NULL;
    return 0;
}

extern void _ojl_sort_int_array(int *cp, int n);

/* Once we have set up a combiner with the appropriate values, we can calculate some
 * things without actually running the iterations. "Rank" is the order in which a
 * given hand would appear in the sequence if we ran it. The "hand_at" function does
 * the reverse: which hand would appear at that rank. These functions use
 * colexicographical order, because that's the simplest to calculate.
 */
long long ojc_colex_rank(oj_cardlist_t *hand, oj_combiner_t *comb) {
    int i, buf[56];
    long long r = 0LL;
    assert(0 != hand && 0 != comb);
    assert(hand->length < 64);

    if (hand->length != comb->k) return -1LL;

    for (i = 0; i < comb->k; ++i) buf[i] = comb->deck_invert[hand->cards[i]];
    _ojl_sort_int_array(buf, comb->k);
    for (i = 0; i < comb->k; ++i) r += ojc_binomial(buf[i], i + 1);
    return r;
}

int ojc_colex_hand_at(long long rank, oj_cardlist_t *hand, oj_combiner_t *comb) {
    int i, buf[64], v = comb->deck->length;
    long long b;
    assert(0 != hand && 0 != comb);

    if (hand->pflags & OJF_RDONLY) return OJE_RDONLY;
    if (hand->allocation < comb->k) return OJE_FULL;
    if (rank >= comb->total) return OJE_BADINDEX;

    for (i = comb->k; i >= 1; --i) {
        while ((b = ojc_binomial(v, i)) > rank) --v;
        buf[i - 1] = v;
        rank -= b;
    }
    hand->length = comb->k;
    for (i = 0; i < comb->k; ++i) hand->cards[i] = comb->deck->cards[buf[i]];
    hand->eflags = 0;
    return 0;
}

/* The two "next" generator functions are used like this:
 *     ojc_new...
 *     while ojc_next... {
 *         ...
 *     }
 */
__attribute__((hot))
int ojc_next(oj_combiner_t *comb) {
    int i, j, *a = comb->map;
    int k = comb->k, n = comb->deck->length;
    assert(0 != comb && 0x10ACE0FF == comb->_johnnymoss);

    if (0 == comb->remaining) return 0;

    if (! (comb->flags & OJF_VALIDRANK)) {
        comb->rank = ojc_colex_rank(comb->hand, comb);
        comb->remaining = comb->total - comb->rank;
        comb->flags |= OJF_VALIDRANK;
    }
    if (comb->remaining != comb->total) {
        for (i = 0; i < k; ++i) {
            if ( ((i < k - 1) && (a[i] < (a[i + 1] - 1))) ||
                  ((i == k - 1) && (a[i] < n - 1)) ) {
                ++a[i];
                for (j = 0; j < i; ++j) a[j] = j;
            }
        }
        ++comb->rank;
        comb->flags &= ~OJF_VALIDMAP;
    }
    for (i = 0; i < k; ++i) {
        comb->hand->cards[i] = comb->deck->cards[a[i]];
    }
    --comb->remaining;
    comb->hand->eflags = 0;
    return 1;
}

__attribute__((hot))
int ojc_next_random(oj_combiner_t *comb) {
    int i, k = comb->k, n = comb->deck->length;
    assert(0 != comb && 0x10ACE0FF == comb->_johnnymoss);

    if (0 == comb->remaining) return 0;

    if (! (comb->flags & OJF_VALIDMAP)) {
        for (i = 0; i < n; ++i) comb->map[i] = i;
        comb->flags |= OJF_VALIDMAP;
    }
    ojr_shuffle(comb->map, n, k);
    comb->flags &= ~OJF_VALIDRANK;

    for (i = 0; i < k; ++i) {
        comb->hand->cards[i] = comb->deck->cards[comb->map[i]];
    }
    --comb->remaining;
    comb->hand->eflags = 0;
    return 1;
}
