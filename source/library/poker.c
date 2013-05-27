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

/* The magic here is in building the lookup tables. See pokertables.py in the
 * python directory. Call this when yoou know the hand is exactly five cards
 * and you want the value quickly.
 */
int ojp_eval5(oj_cardlist_t *sp) {
    return ldc4[ ldc3[ ldc2[ ldc1[
        52 * (sp->cards[0] - 1) + sp->cards[1] ]
           + sp->cards[2] ] + sp->cards[3] ] + sp->cards[4] ];
}

static oj_combiner_t piter;
static oj_cardlist_t phand;
static int hbuf[8];

/* Given a sequence of any length, find the best 5-card hand and its value.
 */
int ojp_best5(oj_cardlist_t *sp, oj_cardlist_t *bh) {
    int v, best;
    assert(0 != sp && sp->length >= 5);
    assert(0 != bh && bh->allocation >= 5 && (!(bh->pflags & OJF_RDONLY)));

    if (5 == sp->length) {
        ojl_copy(bh, sp);
        return ojp_eval5(sp);
    }
    ojl_new(&phand, hbuf, 5);
    ojc_new(&piter, sp, &phand, 5, 0LL);

    best = 9999;
    while (ojc_next(&piter)) {
        v = ojp_eval5(piter.hand);
        if (v < best) {
            best = v;
            ojl_copy(bh, piter.hand);
        }
    }
    return best;
}

/* Collect all the info necessary to display a poker hand and its value in a
 * user-friendly way. This is a big ugly glob of special cases, but there's
 * really not much way to simplify it that I can think of.
 */
int ojp_hand_info(oj_poker_hand_info_t *pi, oj_cardlist_t *sp, int val) {
    int i, t[5];
    assert(0 != pi && 0 != sp);
    assert(5 == sp->length);

    if (-1 == val) val = ojp_eval5(sp);
    pi->val = val;

    ojl_sort(sp);
    ojl_reverse(sp);

    if (val < 11) {
        pi->group = 0;  /* Straight Flush */
        pi->nranks = 1;
        pi->ranks[0] = OJ_RANK(sp->cards[0]);
        if (OJR_ACE == pi->ranks[0] && OJR_FIVE == OJ_RANK(sp->cards[1])) {
            pi->ranks[0] = OJR_FIVE;
            t[0] = sp->cards[0];
            memmove(sp->cards, sp->cards + 1, 4 * sizeof(int));
            sp->cards[4] = t[0];
        }
    } else if (val < 167) {
        pi->group = 1;  /* Quads */
        pi->nranks = 2;
        if (OJ_RANK(sp->cards[0]) != OJ_RANK(sp->cards[1])) {
            t[0] = sp->cards[0];
            memmove(sp->cards, sp->cards + 1, 4 * sizeof(int));
            sp->cards[4] = t[0];
        }
        pi->ranks[0] = OJ_RANK(sp->cards[0]);
        pi->ranks[1] = OJ_RANK(sp->cards[4]);
    } else if (val < 323) {
        pi->group = 2;  /* Full House */
        pi->nranks = 2;
        if (OJ_RANK(sp->cards[0]) != OJ_RANK(sp->cards[2])) {
            memmove(t, sp->cards, 2 * sizeof(int));
            memmove(sp->cards, sp->cards + 2, 3 * sizeof(int));
            memmove(sp->cards + 3, t, 2 * sizeof(int));
        }
        pi->ranks[0] = (sp->cards[0] - 1) >> 2;
        pi->ranks[1] = (sp->cards[3] - 1) >> 2;
    } else if (val < 1600) {
        pi->group = 3;  /* Flush */
        pi->nranks = 5;
        for (i = 0; i < 5; ++i) pi->ranks[i] = OJ_RANK(sp->cards[i]);
    } else if (val < 1610) {
        pi->group = 4;  /* Straight */
        pi->nranks = 1;
        pi->ranks[0] = OJ_RANK(sp->cards[0]);
        if (OJR_ACE == pi->ranks[0] && OJR_FIVE == OJ_RANK(sp->cards[1])) {
            pi->ranks[0] = OJR_FIVE;
            t[0] = sp->cards[0];
            memmove(sp->cards, sp->cards + 1, 4 * sizeof(int));
            sp->cards[4] = t[0];
        }
    } else if (val < 2468) {
        pi->group = 5;  /* Trips */
        pi->nranks = 3;
        for (i = 0; i < 3; ++i) t[i] = OJ_RANK(sp->cards[i]);
        if (t[0] != t[1]) {
            if (t[1] != t[2]) {
                memmove(t, sp->cards, 2 * sizeof(int));
                memmove(sp->cards, sp->cards + 2, 3 * sizeof(int));
                memmove(sp->cards + 3, t, 2 * sizeof(int));
            } else {
                t[0] = sp->cards[0];
                memmove(sp->cards, sp->cards + 1, 3 * sizeof(int));
                sp->cards[3] = t[0];
            }
        }
        for (i = 0; i < 3; ++i) pi->ranks[i] = OJ_RANK(sp->cards[i + 2]);
    } else if (val < 3326) {
        pi->group = 6;  /* Two Pair */
        pi->nranks = 3;
        for (i = 0; i < 5; ++i) t[i] = OJ_RANK(sp->cards[i]);
        if (t[3] == t[4]) {
            if (t[0] == t[1]) {
                t[0] = sp->cards[2];
                memmove(sp->cards + 2, sp->cards + 3, 2 * sizeof(int));
                sp->cards[4] = t[0];
            } else {
                t[0] = sp->cards[0];
                memmove(sp->cards, sp->cards + 1, 4 * sizeof(int));
                sp->cards[4] = t[0];
            }
        }
        for (i = 0; i < 3; ++i) pi->ranks[i] = OJ_RANK(sp->cards[i << 1]);
    } else if (val < 6186) {
        pi->group = 7;  /* One Pair */
        pi->nranks = 4;
        for (i = 0; i < 5; ++i) t[i] = OJ_RANK(sp->cards[i]);
        if (t[1] == t[2]) {
            t[0] = sp->cards[0];
            memmove(sp->cards, sp->cards + 1, 2 * sizeof(int));
            sp->cards[2] = t[0];
        } else if (t[2] == t[3]) {
            memmove(t, sp->cards, 2 * sizeof(int));
            memmove(sp->cards, sp->cards + 2, 2 * sizeof(int));
            memmove(sp->cards + 2, t, 2 * sizeof(int));
        } else if (t[3] == t[4]) {
            memmove(t, sp->cards, 3 * sizeof(int));
            memmove(sp->cards, sp->cards + 3, 2 * sizeof(int));
            memmove(sp->cards + 2, t, 3 * sizeof(int));
        }
        for (i = 0; i < 4; ++i) pi->ranks[i] = OJ_RANK(sp->cards[i + 1]);
    } else {
        pi->group = 8;  /* No Pair */
        pi->nranks = 5;
        for (i = 0; i < 5; ++i) pi->ranks[i] = OJ_RANK(sp->cards[i]);
    }
    pi->_johnnymoss = 0x10ACE0FF;
    return pi->group;
}

static char *_ojp_hand_group_names[] = {
    "Straight Flush", "Four of a Kind", "Full House", "Flush", "Straight",
    "Three of a Kind", "Two Pair", "One Pair", "No Pair"
};

char *ojp_hand_group_name(int g) {
    assert(g >= 0 && g <= 8);
    return _ojp_hand_group_names[g];
}

static char _hdbuf[48];

#define RN(x) (ojt_rank(pi->ranks[x]))
#define RNP(x) ((OJR_SIX==pi->ranks[x])?"es":"s")

char *ojp_hand_description(oj_poker_hand_info_t *pi) {
    assert(0x10ACE0FF == pi->_johnnymoss);

    switch (pi->group) {
    case 0:
        if (12 == pi->ranks[0]) {
            snprintf(_hdbuf, 48, "Royal Flush");
        } else {
            snprintf(_hdbuf, 48, "%s-high Straight Flush", RN(0));
        }
        break;
    case 1:
        snprintf(_hdbuf, 48, "Four %s%s, %s", RN(0), RNP(0), RN(1));
        break;
    case 2:
        snprintf(_hdbuf, 48, "%s%s Full of %s%s", RN(0), RNP(0),
            RN(1), RNP(1));
        break;
    case 3:
        snprintf(_hdbuf, 48, "Flush, %s, %s, %s, %s, %s", RN(0), RN(1),
            RN(2), RN(3), RN(4));
        break;
    case 4:
        snprintf(_hdbuf, 48, "%s-high Straight", RN(0));
        break;
    case 5:
        snprintf(_hdbuf, 48, "Three %s%s, %s, %s", RN(0), RNP(0),
            RN(1), RN(2));
        break;
    case 6:
        snprintf(_hdbuf, 48, "Two Pair, %s%s and %s%s, %s", RN(0), RNP(0),
            RN(1), RNP(1), RN(2));
        break;
    case 7:
        snprintf(_hdbuf, 48, "Pair of %s%s, %s, %s, %s", RN(0), RNP(0),
            RN(1), RN(2), RN(3));
        break;
    case 8:
        snprintf(_hdbuf, 48, "No Pair, %s, %s, %s, %s, %s", RN(0), RN(1),
            RN(2), RN(3), RN(4));
        break;
    }
    return _hdbuf;
}
