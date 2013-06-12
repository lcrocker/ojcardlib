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

// Return (n choose k).
int64_t ojc_binomial(int n, int k) {
    assert(n >= 0 && k >= 0);

    if (0 == k || n == k) return 1LL;
    if (k > n) return 0LL;

    if (k > (n - k)) k = n - k;
    if (1 == k) return (long long)n;

    if (n <= 54 && k <= 54) {
        return bctable[(((n - 3) * (n - 3)) >> 2) + k];
    }
    // Last resort: actually calculate
    int64_t b = 1LL;
    for (int i = 1; i <= k; ++i) {
        b *= (n - (k - i));
        if (b < 0) return -1LL; // Overflow
        b /= i;
    }
    return b;
}

// Initialize a new combiner.
int ojc_new(
    oj_combiner *cp,
    oj_cardlist *deck,
    oj_cardlist *hand,
    int k,
    int64_t count)
{
    int i;

    assert(0 != cp && 0 != deck && 0 != hand);
    assert(hand->allocation >= k);
    assert(deck->length >= k && deck->length <= 54);
    assert(k >= 0 && k <= 54);

    if (hand->pflags & OJF_RDONLY) return OJE_RDONLY;

    cp->_johnnymoss = 0x10ACE0FF;
    cp->deck = deck;
    cp->hand = hand;
    cp->k = k;

    cp->total = ojc_binomial(deck->length, k);
    if (0 == count) cp->remaining = cp->total;
    else cp->remaining = count;

    hand->length = k;
    for (i = 0; i < k; ++i) hand->cards[i] = deck->cards[i];
    for (i = 0; i < deck->length; ++i) {
        cp->map[i] = cp->invert[deck->cards[i]] = i;
    }

    hand->eflags = 0;
    return 0;
}

// Generate next combination in colex order
int ojc_next(oj_combiner *cp) {
    int i, j, k = cp->k, n = cp->deck->length;
    oj_card *a = cp->map;
    assert(0 != cp && 0x10ACE0FF == cp->_johnnymoss);

    if (0 == cp->remaining) return 0;

    if (cp->remaining != cp->total) {
        for (i = 0; i < k-1; ++i) {
            if (a[i] < a[i+1] - 1) break;
        }
        assert(! ((i == k-1) && (a[i] == n-1)));

        ++a[i];
        for (j = 0; j < i; ++j) a[j] = j;
    }
    for (int c = 0; c < k; ++c) {
        cp->hand->cards[c] = cp->deck->cards[a[c]];
    }
    --cp->remaining;
    cp->hand->eflags = 0;
    return 1;
}

int ojc_next_random(oj_combiner *cp) {
    int k = cp->k, n = cp->deck->length;
    oj_card *mp = cp->map;
    assert(0 != cp && 0x10ACE0FF == cp->_johnnymoss);

    if (0 == cp->remaining) return 0;

    // Bob Floyd's algorithm
    int_fast64_t set = 0ll, m;
    for (int j = n - k; j < n; ++j) {
        int r = ojr_rand(j+1);
        m = 1ll << r;

        if (set & m) {
            *mp++ = j;
            set |= (1ll << j);
        } else {
            *mp++ = r;
            set |= m;
        }
    }
    for (int i = 0; i < k; ++i) {
        cp->hand->cards[i] = cp->deck->cards[cp->map[i]];
    }
    --cp->remaining;
    cp->hand->eflags = 0;
    return 1;
}

extern void _ojl_sort_cards(oj_card *cp, int n);

// Return colex rank of given hand
int64_t ojc_colex_rank(oj_combiner *cp, oj_cardlist *hand) {
    oj_card buf[56];
    int64_t r = 0LL;
    assert(0 != hand && 0 != cp);
    assert(hand->length < 64);

    if (hand->length != cp->k) return -1LL;

    for (int i = 0; i < cp->k; ++i) buf[i] = cp->invert[hand->cards[i]];
    _ojl_sort_cards(buf, cp->k);
    for (int i = 0; i < cp->k; ++i) r += ojc_binomial(buf[i], i + 1);
    return r;
}

// Return hand at given rank
int ojc_colex_hand_at(oj_combiner *cp, int64_t rank, oj_cardlist *hand) {
    int i, n = cp->deck->length;
    oj_card buf[64];
    int64_t b;
    assert(0 != hand && 0 != cp);

    if (hand->pflags & OJF_RDONLY) return OJE_RDONLY;
    if (hand->allocation < cp->k) return OJE_FULL;
    if (rank >= cp->total) return OJE_BADINDEX;

    for (i = cp->k; i >= 1; --i) {
        while ((b = ojc_binomial(n, i)) > rank) --n;
        buf[i-1] = n;
        rank -= b;
    }
    hand->length = cp->k;
    for (i = 0; i < cp->k; ++i) hand->cards[i] = cp->deck->cards[buf[i]];
    hand->eflags = 0;
    return 0;
}
