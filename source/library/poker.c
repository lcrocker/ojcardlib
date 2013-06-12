/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Poker functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "ojcardlib.h"
#include "ldctables.h"

// Evaluate five-card poker hand.
int ojp_eval5(oj_cardlist *p) {
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss);
    assert(5 == p->length);

    return ldc4[ ldc3[ ldc2[ ldc1[
        52 * (p->cards[0] - 1) + p->cards[1] ]
           + p->cards[2] ] + p->cards[3] ] + p->cards[4] ];
}

// On my machine, branchless MINs are actually a bit slower
// than the straightforward one.
#define MIN(x,y) (((x)<(y)) ? (x) : (y))

// 7 cards is common enough to deserve special case code.
// Unlike best5(), this won't return the actual hand, but it's faster.
int ojp_eval7(oj_cardlist *p) {
    oj_card *h = p->cards;
    int b0 = 52 * (h[0] - 1);
    int b1 = ldc1[ b0 + h[1] ];
    int b2 = ldc2[ b1 + h[2] ];
    int b3 = ldc3[ b2 + h[3] ];
    int best = ldc4[ b3 + h[4] ];   // 1

    int b4 = ldc4[ b3 + h[5] ];
    best = MIN(best, b4);           // 2

    b4 = ldc4[ b3 + h[6] ];
    best = MIN(best, b4);           // 3

    b3 = ldc3[ b2 + h[4] ];
    b4 = ldc4[ b3 + h[5] ];
    best = MIN(best, b4);           // 4

    b4 = ldc4[ b3 + h[6] ];
    best = MIN(best, b4);           // 5

    b3 = ldc3[ b2 + h[5] ];
    b4 = ldc4[ b3 + h[6] ];
    best = MIN(best, b4);           // 6

    b2 = ldc2[ b1 + h[3] ];
    b3 = ldc3[ b2 + h[4] ];
    b4 = ldc4[ b3 + h[5] ];
    best = MIN(best, b4);           // 7

    b4 = ldc4[ b3 + h[6] ];
    best = MIN(best, b4);           // 8

    b3 = ldc3[ b2 + h[5] ];
    b4 = ldc4[ b3 + h[6] ];
    best = MIN(best, b4);           // 9

    b2 = ldc2[ b1 + h[4] ];
    b3 = ldc3[ b2 + h[5] ];
    b4 = ldc4[ b3 + h[6] ];
    best = MIN(best, b4);           // 10

    b1 = ldc1[ b0 + h[2] ];
    b2 = ldc2[ b1 + h[3] ];
    b3 = ldc3[ b2 + h[4] ];
    b4 = ldc4[ b3 + h[5] ];
    best = MIN(best, b4);           // 11

    b4 = ldc4[ b3 + h[6] ];
    best = MIN(best, b4);           // 12

    b3 = ldc3[ b2 + h[5] ];
    b4 = ldc4[ b3 + h[6] ];
    best = MIN(best, b4);           // 13

    b2 = ldc2[ b1 + h[4] ];
    b3 = ldc3[ b2 + h[5] ];
    b4 = ldc4[ b3 + h[6] ];
    best = MIN(best, b4);           // 14

    b1 = ldc1[ b0 + h[3] ];
    b2 = ldc2[ b1 + h[4] ];
    b3 = ldc3[ b2 + h[5] ];
    b4 = ldc4[ b3 + h[6] ];
    best = MIN(best, b4);           // 15

    b0 = 52 * (h[1] - 1);
    b1 = ldc1[ b0 + h[2] ];
    b2 = ldc2[ b1 + h[3] ];
    b3 = ldc3[ b2 + h[4] ];
    b4 = ldc4[ b3 + h[5] ];
    best = MIN(best, b4);           // 16

    b4 = ldc4[ b3 + h[6] ];
    best = MIN(best, b4);           // 17

    b3 = ldc3[ b2 + h[5] ];
    b4 = ldc4[ b3 + h[6] ];
    best = MIN(best, b4);           // 18

    b2 = ldc2[ b1 + h[4] ];
    b3 = ldc3[ b2 + h[5] ];
    b4 = ldc4[ b3 + h[6] ];
    best = MIN(best, b4);           // 19

    b1 = ldc1[ b0 + h[3] ];
    b2 = ldc2[ b1 + h[4] ];
    b3 = ldc3[ b2 + h[5] ];
    b4 = ldc4[ b3 + h[6] ];
    best = MIN(best, b4);           // 20

    b0 = 52 * (h[2] - 1);
    b1 = ldc1[ b0 + h[3] ];
    b2 = ldc2[ b1 + h[4] ];
    b3 = ldc3[ b2 + h[5] ];
    b4 = ldc4[ b3 + h[6] ];
    best = MIN(best, b4);           // 21

    return best;
}

static oj_combiner _cmb;
static oj_cardlist _hand;
static oj_card _hbuf[8];

// Given a sequence of any length, find the best 5-card hand and its value.
int ojp_best5(oj_cardlist *p, oj_cardlist *bh) {
    assert(0 != p && p->length >= 5);
    assert(0 != bh && bh->allocation >= 5 && (!(bh->pflags & OJF_RDONLY)));

    if (5 == p->length) {
        ojl_copy(bh, p);
        return ojp_eval5(p);
    }
    ojl_new(&_hand, _hbuf, 5);
    ojc_new(&_cmb, p, &_hand, 5, 0LL);

    int best = 9999;
    while (ojc_next(&_cmb)) {
        int v = ojp_eval5(&_hand);
        if (v < best) {
            best = v;
            ojl_copy(bh, &_hand);
        }
    }
    return best;
}

// Collect all the info necessary to display a poker hand and its value in a
// user-friendly way. This is a big ugly glob of special cases, but there's
// really not much way to simplify it.
int ojp_hand_info(oj_poker_hand_info *pi, oj_cardlist *p, int val) {
    oj_card t[5], *h = p->cards;
    assert(0 != pi && 0 != p);
    assert(5 == p->length);

    if (-1 == val) val = ojp_eval5(p);
    pi->val = val;

    ojl_sort(p);
    ojl_reverse(p);

    if (val < 11) {
        pi->group = 1;  // Straight Flush
        pi->nranks = 1;
        pi->ranks[0] = OJ_RANK(h[0]);
        if (OJR_ACE == pi->ranks[0] && OJR_FIVE == OJ_RANK(h[1])) {
            pi->ranks[0] = OJR_FIVE;
            t[0] = h[0];
            memmove(h, h + 1, 4 * sizeof(oj_card));
            h[4] = t[0];
        }
    } else if (val < 167) {
        pi->group = 2;  // Quads
        pi->nranks = 2;
        if (OJ_RANK(h[0]) != OJ_RANK(h[1])) {
            t[0] = h[0];
            memmove(h, h + 1, 4 * sizeof(oj_card));
            h[4] = t[0];
        }
        pi->ranks[0] = OJ_RANK(h[0]);
        pi->ranks[1] = OJ_RANK(h[4]);
    } else if (val < 323) {
        pi->group = 3;  // Full House
        pi->nranks = 2;
        if (OJ_RANK(h[0]) != OJ_RANK(h[2])) {
            memmove(t, h, 2 * sizeof(oj_card));
            memmove(h, h + 2, 3 * sizeof(oj_card));
            memmove(h + 3, t, 2 * sizeof(oj_card));
        }
        pi->ranks[0] = OJ_RANK(h[0]);
        pi->ranks[1] = OJ_RANK(h[3]);
    } else if (val < 1600) {
        pi->group = 4;  // Flush
        pi->nranks = 5;
        for (int i = 0; i < 5; ++i) pi->ranks[i] = OJ_RANK(h[i]);
    } else if (val < 1610) {
        pi->group = 5;  // Straight
        pi->nranks = 1;
        pi->ranks[0] = OJ_RANK(h[0]);
        if (OJR_ACE == pi->ranks[0] && OJR_FIVE == OJ_RANK(h[1])) {
            pi->ranks[0] = OJR_FIVE;
            t[0] = h[0];
            memmove(h, h + 1, 4 * sizeof(oj_card));
            h[4] = t[0];
        }
    } else if (val < 2468) {
        pi->group = 6;  // Trips
        pi->nranks = 3;
        for (int i = 0; i < 3; ++i) t[i] = OJ_RANK(h[i]);
        if (t[0] != t[1]) {
            if (t[1] != t[2]) {
                memmove(t, h, 2 * sizeof(oj_card));
                memmove(h, h + 2, 3 * sizeof(oj_card));
                memmove(h + 3, t, 2 * sizeof(oj_card));
            } else {
                t[0] = h[0];
                memmove(h, h + 1, 3 * sizeof(oj_card));
                h[3] = t[0];
            }
        }
        for (int i = 0; i < 3; ++i) pi->ranks[i] = OJ_RANK(h[i+2]);
    } else if (val < 3326) {
        pi->group = 7;  // Two Pair
        pi->nranks = 3;
        for (int i = 0; i < 5; ++i) t[i] = OJ_RANK(h[i]);
        if (t[3] == t[4]) {
            if (t[0] == t[1]) {
                t[0] = h[2];
                memmove(h + 2, h + 3, 2 * sizeof(oj_card));
                h[4] = t[0];
            } else {
                t[0] = h[0];
                memmove(h, h + 1, 4 * sizeof(oj_card));
                h[4] = t[0];
            }
        }
        for (int i = 0; i < 3; ++i) pi->ranks[i] = OJ_RANK(h[i << 1]);
    } else if (val < 6186) {
        pi->group = 8;  // One Pair
        pi->nranks = 4;
        for (int i = 0; i < 5; ++i) t[i] = OJ_RANK(h[i]);
        if (t[1] == t[2]) {
            t[0] = h[0];
            memmove(h, h + 1, 2 * sizeof(oj_card));
            h[2] = t[0];
        } else if (t[2] == t[3]) {
            memmove(t, h, 2 * sizeof(oj_card));
            memmove(h, h + 2, 2 * sizeof(oj_card));
            memmove(h + 2, t, 2 * sizeof(oj_card));
        } else if (t[3] == t[4]) {
            memmove(t, h, 3 * sizeof(oj_card));
            memmove(h, h + 3, 2 * sizeof(oj_card));
            memmove(h + 2, t, 3 * sizeof(oj_card));
        }
        for (int i = 0; i < 4; ++i) pi->ranks[i] = OJ_RANK(h[i+1]);
    } else {
        pi->group = 9;  // No Pair
        pi->nranks = 5;
        for (int i = 0; i < 5; ++i) pi->ranks[i] = OJ_RANK(h[i]);
    }
    pi->_johnnymoss = 0x10ACE0FF;
    return pi->group;
}

static char *_ojp_hand_group_names[] = {
    NULL, "Straight Flush", "Four of a Kind", "Full House", "Flush",
    "Straight", "Three of a Kind", "Two Pair", "One Pair", "No Pair"
};

char *ojp_hand_group_name(int g) {
    assert(g >= 0 && g <= 8);
    return _ojp_hand_group_names[g];
}

#define RN(x) (ojt_rank(pi->ranks[x]))
#define RNP(x) ((OJR_SIX==pi->ranks[x])?"es":"s")

char *ojp_hand_description(oj_poker_hand_info *pi, char *buf, int size) {
    assert(0x10ACE0FF == pi->_johnnymoss);

    switch (pi->group) {
    case 0:
        if (12 == pi->ranks[0]) {
            snprintf(buf, size, "Royal Flush");
        } else {
            snprintf(buf, size, "%s-high Straight Flush", RN(0));
        }
        break;
    case 1:
        snprintf(buf, size, "Four %s%s, %s", RN(0), RNP(0), RN(1));
        break;
    case 2:
        snprintf(buf, size, "%s%s Full of %s%s", RN(0), RNP(0),
            RN(1), RNP(1));
        break;
    case 3:
        snprintf(buf, size, "Flush, %s, %s, %s, %s, %s", RN(0), RN(1),
            RN(2), RN(3), RN(4));
        break;
    case 4:
        snprintf(buf, size, "%s-high Straight", RN(0));
        break;
    case 5:
        snprintf(buf, size, "Three %s%s, %s, %s", RN(0), RNP(0),
            RN(1), RN(2));
        break;
    case 6:
        snprintf(buf, size, "Two Pair, %s%s and %s%s, %s", RN(0), RNP(0),
            RN(1), RNP(1), RN(2));
        break;
    case 7:
        snprintf(buf, size, "Pair of %s%s, %s, %s, %s", RN(0), RNP(0),
            RN(1), RN(2), RN(3));
        break;
    case 8:
        snprintf(buf, size, "No Pair, %s, %s, %s, %s, %s", RN(0), RN(1),
            RN(2), RN(3), RN(4));
        break;
    }
    return buf;
}
