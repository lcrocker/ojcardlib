/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * A cardlist is simply any ordered sequence of cards. These functions
 * are loosely modeled after Python list methods.
 *
 * List functions generally return 0 or a meaningful positive number if
 * successful and a negative OJE_ error code if not. Note that the macro
 * ER() below can be used to make functions fail and exit rather than
 * returning an error if you need to do some serious debugging. This will
 * affect the Java and Python bindings and make the test suite fail, so
 * only do that for a session or if you're linking statically.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

#include "ojcardlib.h"


#define ER(c) return(c)
/*
#define ER(c) assert(0)
*/

/* On most modern processors, big shifts like 1LL << 40 are faster than a
 * table lookup, but the latter is likely to be faster on some machines, so
 * we have both here. Uncomment whichever version of the M() macro works
 * better for you.
 */
/*
static long long _masks[] = { 0LL,
    1LL<< 1, 1LL<< 2, 1LL<< 3, 1LL<< 4, 1LL<< 5, 1LL<< 6, 1LL<< 7, 1LL<< 8,
    1LL<< 9, 1LL<<10, 1LL<<11, 1LL<<12, 1LL<<13, 1LL<<14, 1LL<<15, 1LL<<16,
    1LL<<17, 1LL<<18, 1LL<<19, 1LL<<20, 1LL<<21, 1LL<<22, 1LL<<23, 1LL<<24,
    1LL<<25, 1LL<<26, 1LL<<27, 1LL<<28, 1LL<<29, 1LL<<30, 1LL<<31, 1LL<<32,
    1LL<<33, 1LL<<34, 1LL<<35, 1LL<<36, 1LL<<37, 1LL<<38, 1LL<<39, 1LL<<40,
    1LL<<41, 1LL<<42, 1LL<<43, 1LL<<44, 1LL<<45, 1LL<<46, 1LL<<47, 1LL<<48,
    1LL<<49, 1LL<<50, 1LL<<51, 1LL<<52, 1LL<<53, 1LL<<54,
};
#define M(c) (_masks[c])
*/
#define M(c) (1LL<<(c))

#define ISSET(m,c) (0LL != ((m) & M(c)))
#define SET(m,c) ((m) |= M(c))
#define CLEAR(m,c) ((m) &= ~M(c))

/* Initialize an empty list object with the given buffer.
*/
int ojl_new(oj_cardlist_t *sp, int *buf, int size) {
    assert(0 != sp && 0 != size && 0 != buf);

    memset(sp, 0, sizeof(oj_cardlist_t));
    sp->_johnnymoss = 0x10ACE0FF;
    sp->allocation = size;
    sp->cards = buf;
    return size;
}

/* Build a 64-bit mask value for the give cardlist. Return the number of
 * duplicates found.
 */
int ojl_build_mask(oj_cardlist_t *sp, uint64_t *mp) {
    int i, dups = 0;
    assert(0 != mp && 0 != sp && 0x10ACE0FF == sp->_johnnymoss);

    *mp = 0LL;
    for (i = 0; i < sp->length; ++i) {
        if (ISSET(*mp, sp->cards[i])) ++dups;
        SET(*mp, sp->cards[i]);
    }
    return dups;
}

/* Remove all cards.
 */
int ojl_clear(oj_cardlist_t *sp) {
    assert(0 != sp && 0x10ACE0FF == sp->_johnnymoss);
    if (sp->pflags & OJF_RDONLY) ER(OJE_RDONLY);

    sp->length = sp->eflags = 0;
    sp->mask = 0LL;
    return 0;
}

/* Python doesn't have this one, but it's handy for our uses. Remove all but
 * first <size> cards.
 */
int ojl_truncate(oj_cardlist_t *sp, int size) {
    assert(0 != sp && 0x10ACE0FF == sp->_johnnymoss);
    if (sp->pflags & OJF_RDONLY) ER(OJE_RDONLY);

    if (size > sp->length) ER(OJE_BADINDEX);
    if (size == sp->length) return 0;

    sp->length = size;
    sp->eflags = 0;
    if (sp->pflags & OJF_UNIQUE) ojl_build_mask(sp, &sp->mask);
    return 0;
}

int ojl_size(oj_cardlist_t *sp) { return sp->length; }

/* These get/put functions pay attention to flags and error checking.
 * Accessing the cards array directly can be used for performance if that
 * isn't needed. Language bindings should use these as well.
 */
int ojl_get(oj_cardlist_t *sp, int index) {
    assert(0 != sp && 0x10ACE0FF == sp->_johnnymoss);
    if (index >= sp->length) ER(OJE_BADINDEX);
    return sp->cards[index];
}

int ojl_set(oj_cardlist_t *sp, int index, int card) {
    int oc;
    assert(0 != sp && 0x10ACE0FF == sp->_johnnymoss);
    if (sp->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    if (index >= sp->length) ER(OJE_BADINDEX);

    oc = sp->cards[index];
    if (oc == card) return oc;

    if (sp->pflags & OJF_UNIQUE) {
        if (ISSET(sp->mask, card)) ER(OJE_DUPLICATE);
        CLEAR(sp->mask, oc);
        SET(sp->mask, card);
    }
    sp->cards[index] = card;
    sp->eflags = 0;
    return oc;
}

/* Read, set, and clear flags. If a list is read only, you can only change
 * the read-only flag.
 */
int ojl_pflag(oj_cardlist_t *sp, int mask) { return sp->pflags & mask; }
int ojl_eflag(oj_cardlist_t *sp, int mask) { return sp->eflags & mask; }

int ojl_set_pflag(oj_cardlist_t *sp, int mask) {
    assert(0 != sp && 0x10ACE0FF == sp->_johnnymoss);
    if ((sp->pflags & OJF_RDONLY) && (OJF_RDONLY != mask)) ER(OJE_RDONLY);

    sp->pflags |= mask;
    return 0;
}

int ojl_clear_pflag(oj_cardlist_t *sp, int mask) {
    assert(0 != sp && 0x10ACE0FF == sp->_johnnymoss);
    if ((sp->pflags & OJF_RDONLY) && (OJF_RDONLY != mask)) ER(OJE_RDONLY);

    sp->pflags &= (~mask);
    return 0;
}

int ojl_set_eflag(oj_cardlist_t *sp, int mask) {
    assert(0 != sp && 0x10ACE0FF == sp->_johnnymoss);
    if (sp->pflags & OJF_RDONLY) ER(OJE_RDONLY);

    sp->eflags |= mask;
    return 0;
}

int ojl_clear_eflag(oj_cardlist_t *sp, int mask) {
    assert(0 != sp && 0x10ACE0FF == sp->_johnnymoss);
    if (sp->pflags & OJF_RDONLY) ER(OJE_RDONLY);

    sp->eflags &= (~mask);
    return 0;
}

/* User data pointer can be used to store whatever an application needs it for.
 */
void *ojl_get_extra(oj_cardlist_t *sp) { return sp->extra; }
void ojl_set_extra(oj_cardlist_t *sp, void *udp) { sp->extra = udp; }

/* Fowler / Noll / Vo 1a: <http://www.isthe.com/chongo/tech/comp/fnv/>
 * Like ojl_equal(), this functions differs for non-duplicate lists.
 */
uint32_t ojl_hash(oj_cardlist_t *sp) {
    int i;
    char *cp;
    uint32_t h = 2166136261U;

    if (sp->pflags & OJF_UNIQUE) {
        cp = (char *)(& sp->mask);
        for (i = 0; i < 7; ++i) {
            h ^= *cp++;
            h *= 16777619;
        }
    } else {
        for (i = 0; i < sp->length; ++i) {
            h ^= sp->cards[i];
            h *= 16777619;
        }
    }
    return h;
}

__attribute__((hot))
int ojl_append(oj_cardlist_t *sp, int card) {
    assert(0 != sp && 0x10ACE0FF == sp->_johnnymoss);
    assert(card > 0 && card <= 54);

    if (sp->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    if (sp->length == sp->allocation) ER(OJE_FULL);

    if (sp->pflags & OJF_UNIQUE) {
        if (ISSET(sp->mask, card)) ER(OJE_DUPLICATE);
        SET(sp->mask, card);
    }
    sp->cards[sp->length++] = card;
    sp->eflags = 0;
    return card;
}

/* Add cards from one list to the end of another.
 */
int ojl_extend(oj_cardlist_t *destp, oj_cardlist_t *srcp, int count) {
    int i, c, r;
    assert(0 != srcp && 0x10ACE0FF == srcp->_johnnymoss);
    assert(0 != destp && 0x10ACE0FF == destp->_johnnymoss);

    if (0 == count) count = srcp->length;
    else if (count > srcp->length) count = srcp->length;
    if (0 == count) return 0;

    if (destp->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    if (count > (destp->allocation - destp->length)) ER(OJE_FULL);

    if (destp->pflags & OJF_UNIQUE) {
        c = 0;
        for (i = 0; i < count; ++i) {
            r = ojl_append(destp, srcp->cards[i]);
            if (OJE_DUPLICATE == r) ER(OJE_DUPLICATE);
            if (r > 0) ++c;
        }
        if (0 != c) srcp->eflags = 0;
        return c;
    } else {
        memmove(destp->cards + destp->length, srcp->cards, count * sizeof(int));
        destp->length += count;
    }
    srcp->eflags = 0;
    return count;
}

/* Insert a card at a specific index.
 */
int ojl_insert(oj_cardlist_t *sp, int index, int card) {
    assert(0 != sp && 0x10ACE0FF == sp->_johnnymoss && index >= 0);
    assert(card > 0 && card <= 54);

    if (sp->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    if (sp->length == sp->allocation) ER(OJE_FULL);
    if (index > sp->length) ER(OJE_BADINDEX);

    if (sp->pflags & OJF_UNIQUE) {
        if (ISSET(sp->mask, card)) ER(OJE_DUPLICATE);
        SET(sp->mask, card);
    }
    memmove(sp->cards + index + 1, sp->cards + index,
        (sp->length - index) * sizeof(int));
    sp->cards[index] = card;
    sp->eflags = 0;
    return ++sp->length;
}

/* Here's a bit of a deviation from the Python model: pop() does not
 * take an index, but that functionality is done with delete(), which
 * returns the value deleted.
 */
__attribute__((hot))
int ojl_pop(oj_cardlist_t * sp) {
    int s;
    assert(0 != sp && 0x10ACE0FF == sp->_johnnymoss);

    if (sp->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    if (0 == sp->length) ER(OJE_BADINDEX);
    s = --sp->length;

    if (sp->pflags & OJF_UNIQUE) CLEAR(sp->mask, sp->cards[s]);
    sp->eflags = 0;
    return sp->cards[s];
}

/* Remove the card at index.
 */
int ojl_delete(oj_cardlist_t * sp, int index) {
    int v;
    assert(0 != sp && 0x10ACE0FF == sp->_johnnymoss && index >= 0);

    if (sp->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    if (index >= sp->length) ER(OJE_BADINDEX);

    v = sp->cards[index];
    if (sp->pflags & OJF_UNIQUE) CLEAR(sp->mask, v);
    --sp->length;

    memmove(sp->cards + index, sp->cards + index + 1,
        (sp->length - index) * sizeof(int));
    sp->eflags = 0;
    return v;
}

/* Return the index of the given card, if found.
 */
int ojl_index(oj_cardlist_t * sp, int card) {
    int i;
    assert(0 != sp && 0x10ACE0FF == sp->_johnnymoss);
    assert(card > 0 && card <= 54);

    for (i = 0; i < sp->length; ++i) {
        if (card == sp->cards[i]) return i;
    }
    return OJE_NOTFOUND; /* Not really an error, so don't wrap in ER() */
}

/* Remove the given card if found.
 */
int ojl_remove(oj_cardlist_t * sp, int card) {
    int i;
    assert(0 != sp && 0x10ACE0FF == sp->_johnnymoss);
    assert(card > 0 && card <= 54);

    if (sp->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    i = ojl_index(sp, card);
    if (OJE_NOTFOUND == i) ER(OJE_NOTFOUND);
    return ojl_delete(sp, i);
}

/* Copy one whole hand onto another, overwriting it.
 */

int ojl_copy(oj_cardlist_t *destp, oj_cardlist_t *srcp) {
    assert(0 != srcp && 0x10ACE0FF == srcp->_johnnymoss);
    assert(0 != destp && 0x10ACE0FF == destp->_johnnymoss);

    if (destp->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    if (destp->allocation < srcp->length) ER(OJE_FULL);

    memmove(destp->cards, srcp->cards, srcp->length * sizeof(int));
    destp->length = srcp->length;
    destp->mask = srcp->mask;
    destp->eflags = 0;
    return 0;
}

/* Many applications involve sorting small hands inside a loop, so we go to
 * some effort here to optimize the hell out of those special cases.
 */

#define SWAP(a,b) do{t=cp[a];cp[a]=cp[b];cp[b]=t;}while(0)
#define CMP(a,b) (cp[a]>cp[b])
#define CSWP(a,b) do{s=cp[a]+cp[b];d=abs(cp[a]-cp[b]);cp[a]=(s-d)>>1;cp[b]=(s+d)>>1;}while(0)

static inline void heapify(int * cp, int n, int start) {
    int t, lc, rc, head;
    int last = ((n + 1) >> 1);

    while (start < last) {
        lc = (start << 1) + 1;
        rc = lc + 1;

        if (rc <= n) {
            if (CMP(rc, lc)) head = rc;
            else head = lc;
        } else head = lc;

        if (CMP(head, start)) {
            SWAP(head, start);
            start = head;
        } else break;
    }
}

void _ojl_sort_int_array(int *cp, int n) {
    int i, s, d, t;

    switch (n) {
    case 0:
    case 1:
        return;
    case 2:
        CSWP(0, 1);
        return;
    case 3:
        CSWP(1, 2); CSWP(0, 2); CSWP(0, 1);
        return;
    case 4:
        CSWP(0, 1); CSWP(2, 3); CSWP(0, 2); CSWP(1, 3); CSWP(1, 2);
        return;
    case 5:
        CSWP(0, 1); CSWP(3, 4); CSWP(2, 4); CSWP(2, 3); CSWP(0, 3);
        CSWP(0, 2); CSWP(1, 4); CSWP(1, 3); CSWP(1, 2);
        return;
    default:
        break;
        /* Fall back to normal in-place heapsort */
    }
    for (i = (n - 1) >> 1; i >= 0; --i) heapify(cp, n - 1, i);
    for (i = n - 1; i > 0; --i) {
        SWAP(0, i);
        heapify(cp, i - 1, 0);
    }
}

/* Sort cards in ascending order.
 */
int ojl_sort(oj_cardlist_t *sp) {
    assert(0 != sp && 0x10ACE0FF == sp->_johnnymoss);
    if (sp->pflags & OJF_RDONLY) ER(OJE_RDONLY);

    _ojl_sort_int_array(sp->cards, sp->length);
    sp->eflags = OJF_SORTED;
    return 0;
}

/* Reverse order of cards.
 */
int ojl_reverse(oj_cardlist_t * sp) {
    int i, t, *cp = sp->cards;
    assert(0 != sp && 0x10ACE0FF == sp->_johnnymoss);
    if (sp->pflags & OJF_RDONLY) ER(OJE_RDONLY);

    for (i = 0; i < (sp->length >> 1); ++i) {
        SWAP(i, (sp->length - 1) - i);
    }
    sp->eflags = 0;
    return 0;
}

__attribute__((hot))
int ojl_equal(oj_cardlist_t * sp1, oj_cardlist_t * sp2) {
    int i;

    if (sp1->pflags & sp2->pflags & OJF_UNIQUE)
        return sp1->mask == sp2->mask;

    if (sp1->length != sp2->length) return 0;
    for (i = 0; i < sp1->length; ++i) {
        if (sp1->cards[i] != sp2->cards[i]) return 0;
    }
    return 1;
}

/* Fill a sequence with fresh cards based on deck type. Can be used to fill
 * multi-deck shoes as well. Removes any existing contents.
 */
int ojl_fill(oj_cardlist_t *sp, int count, oj_deck_type_t dt) {
    oj_cardlist_t *dp = ojd_deck(dt);
    int c, remaining;
    assert(0 != sp && 0x10ACE0FF == sp->_johnnymoss);
    assert(0 != dp && 0x10ACE0FF == dp->_johnnymoss);

    if (sp->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    if (count > sp->allocation) ER(OJE_FULL);
    if ((sp->pflags & OJF_UNIQUE) && (count > dp->length)) ER(OJE_DUPLICATE);

    remaining = count;
    sp->length = 0;
    do {
        c = remaining;
        if (c > dp->length) c = dp->length;
        memmove(sp->cards + sp->length, dp->cards, c * sizeof(int));

        sp->length += c;
        remaining -= c;
    } while (remaining);

    if (sp->pflags & OJF_UNIQUE) ojl_build_mask(sp, &sp->mask);
    sp->eflags = 0;
    return sp->length;
}

/* TODO: implement simultaneous fill-and-shuffle using inside-out F-Y? */

/* Standard Fisher-Yates shuffle.
 */
int ojl_shuffle(oj_cardlist_t * sp) {
    assert(0 != sp && 0x10ACE0FF == sp->_johnnymoss);
    if (sp->pflags & OJF_RDONLY) ER(OJE_RDONLY);

    ojr_shuffle(sp->cards, sp->length, sp->length);
    sp->eflags = 0;
    return 0;
}

/* Put a string representation of the card sequence into the given buffer,
 * clipping if necessary. Return the buffer, or NULL if it was too small for
 * anything useful.
 */
static int _minsize[] = { 3, 5, 8 };

char *ojl_text(oj_cardlist_t *sp, char *buf, int bsize) {
    int i, sl = sp->length, last = 0, clipat = sl + 1;
    char *cn, *bp = buf;

    if ((sl <= 2 && bsize < _minsize[sl]) ||
        (sl > 2 && bsize < 10)) return NULL;
    if (bsize < (3 * sl + 2)) clipat = (bsize - 7) / 3;

    *bp++ = '(';
    last = 0;
    for (i = 0; i < sl; ++i) {
        last = (i == (sl - 1));
        if ((!last) && (i >= clipat)) continue;

        if (last && i >= clipat) {
            *bp++ = '.'; *bp++ = '.'; *bp++ = '.';
        } else if (0 != i) {
            *bp++ = ' ';
        }
        cn = ojt_card(sp->cards[i]);
        *bp++ = *cn++;
        *bp++ = *cn;
    }
    *bp++ = ')';
    *bp = 0;
    assert((bp - buf) < bsize);
    return buf;
}
