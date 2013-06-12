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

// Build a 64-bit mask value for the give cardlist. Return the number of duplicates found.
static int _build_mask(oj_cardlist *p, uint64_t *mp) {
    int i, dups = 0;
    assert(0 != p && 0 != mp && 0x10ACE0FF == p->_johnnymoss);

    *mp = 0LL;
    for (i = 0; i < p->length; ++i) {
        if (ISSET(*mp, p->cards[i])) ++dups;
        SET(*mp, p->cards[i]);
    }
    return dups;
}

// Initialize an empty list object with the given buffer.
int ojl_new(oj_cardlist *p, oj_card *buf, int size) {
    assert(0 != p && 0 != size && 0 != buf);

    memset(p, 0, sizeof(oj_cardlist));
    p->_johnnymoss = 0x10ACE0FF;
    p->allocation = size;
    p->cards = buf;
    return size;
}

// Read flags
int ojl_pflag(oj_cardlist *p, int mask) { return p->pflags & mask; }
int ojl_eflag(oj_cardlist *p, int mask) { return p->eflags & mask; }

// Set flags
int ojl_set_pflag(oj_cardlist *p, int mask) {
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss);
    if ((p->pflags & OJF_RDONLY) && (OJF_RDONLY != mask)) ER(OJE_RDONLY);

    p->pflags |= mask;
    return 0;
}

int ojl_set_eflag(oj_cardlist *p, int mask) {
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss);
    if (p->pflags & OJF_RDONLY) ER(OJE_RDONLY);

    p->eflags |= mask;
    return 0;
}

// Clear flags
int ojl_clear_pflag(oj_cardlist *p, int mask) {
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss);
    if ((p->pflags & OJF_RDONLY) && (OJF_RDONLY != mask)) ER(OJE_RDONLY);

    p->pflags &= (~mask);
    return 0;
}
int ojl_clear_eflag(oj_cardlist *p, int mask) {
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss);
    if (p->pflags & OJF_RDONLY) ER(OJE_RDONLY);

    p->eflags &= (~mask);
    return 0;
}

// Basic get and set. There are macro versions of these for maximum speed,
// but these do better error checking.
oj_card ojl_get(oj_cardlist *p, int index) {
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss);
    if (index >= p->length) ER(OJE_BADINDEX);
    return p->cards[index];
}

oj_card ojl_set(oj_cardlist *p, int index, oj_card card) {
    oj_card oc;
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss);
    if (p->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    if (index >= p->length) ER(OJE_BADINDEX);

    oc = p->cards[index];
    if (oc == card) return oc;

    if (p->pflags & OJF_UNIQUE) {
        if (ISSET(p->mask, card)) ER(OJE_DUPLICATE);
        CLEAR(p->mask, oc);
        SET(p->mask, card);
    }
    p->cards[index] = card;
    p->eflags = 0;
    return oc;
}

// Remove all cards.
int ojl_clear(oj_cardlist *p) {
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss);
    if (p->pflags & OJF_RDONLY) ER(OJE_RDONLY);

    p->length = p->eflags = 0;
    p->mask = 0LL;
    return 0;
}

// Remove all but first <size> cards.
int ojl_truncate(oj_cardlist *p, int size) {
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss);
    if (p->pflags & OJF_RDONLY) ER(OJE_RDONLY);

    if (size > p->length) ER(OJE_BADINDEX);
    if (size == p->length) return 0;

    p->length = size;
    p->eflags = 0;
    if (p->pflags & OJF_UNIQUE) _build_mask(p, &p->mask);
    return 0;
}

int ojl_size(oj_cardlist *p) { return p->length; }

int ojl_equal(oj_cardlist *p1, oj_cardlist *p2) {
    assert(0 != p1 && 0x10ACE0FF == p1->_johnnymoss);
    assert(0 != p2 && 0x10ACE0FF == p2->_johnnymoss);

    int f1 = p1->pflags & OJF_UNIQUE;
    int f2 = p2->pflags & OJF_UNIQUE;

    if (f1 != f2) return 0;
    if (f1 && f2) return p1->mask == p2->mask;
    if (p1->length != p2->length) return 0;

    for (int i = 0; i < p1->length; ++i) {
        if (p1->cards[i] != p2->cards[i]) return 0;
    }
    return 1;
}

// Fowler / Noll / Vo 1a: <http://www.isthe.com/chongo/tech/comp/fnv/>
uint32_t ojl_hash(oj_cardlist *p) {
    int i;
    char *cp;
    uint32_t h = 2166136261U;

    if (p->pflags & OJF_UNIQUE) {
        cp = (char *)(& p->mask);
        for (i = 0; i < 7; ++i) {
            h ^= *cp++;
            h *= 16777619;
        }
    } else {
        for (i = 0; i < p->length; ++i) {
            h ^= p->cards[i];
            h *= 16777619;
        }
    }
    return h;
}

// Add a card to the end of list
oj_card ojl_append(oj_cardlist *p, oj_card card) {
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss);
    assert(card > 0 && card <= 54);

    if (p->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    if (p->length == p->allocation) ER(OJE_FULL);

    if (p->pflags & OJF_UNIQUE) {
        if (ISSET(p->mask, card)) ER(OJE_DUPLICATE);
        SET(p->mask, card);
    }
    p->cards[p->length++] = card;
    p->eflags = 0;
    return card;
}

// Remove and return card at end of list.
oj_card ojl_pop(oj_cardlist *p) {
    int s;
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss);

    if (p->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    if (0 == p->length) ER(OJE_BADINDEX);
    s = --p->length;

    if (p->pflags & OJF_UNIQUE) CLEAR(p->mask, p->cards[s]);
    p->eflags = 0;
    return p->cards[s];
}

// Remove and return a randomly-selected card.
oj_card ojl_pop_random(oj_cardlist *p) {
    int c, r;
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss);

    if (p->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    if (0 == p->length) ER(OJE_BADINDEX);
    r = ojr_rand(p->length);
    c = p->cards[r];

    if (p->pflags & OJF_UNIQUE) CLEAR(p->mask, c);
    p->eflags = 0;
    p->cards[r] = p->cards[--p->length];
    return c;
}

// Return the index of the given card, if found.
int ojl_index(oj_cardlist *p, oj_card card) {
    int i;
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss);
    assert(card > 0 && card <= 54);

    for (i = 0; i < p->length; ++i) {
        if (card == p->cards[i]) return i;
    }
    return OJE_NOTFOUND; // Not really an error, so don't wrap in ER()
}

// Insert a card at a specific index.
int ojl_insert(oj_cardlist *p, int index, oj_card card) {
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss && index >= 0);
    assert(card > 0 && card <= 54);

    if (p->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    if (p->length == p->allocation) ER(OJE_FULL);
    if (index > p->length) ER(OJE_BADINDEX);

    if (p->pflags & OJF_UNIQUE) {
        if (ISSET(p->mask, card)) ER(OJE_DUPLICATE);
        SET(p->mask, card);
    }
    memmove(p->cards + index + 1, p->cards + index,
        (p->length - index) * sizeof(oj_card));
    p->cards[index] = card;
    p->eflags = 0;
    return ++p->length;
}

// Remove the card at index.
oj_card ojl_delete(oj_cardlist *p, int index) {
    int v;
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss && index >= 0);

    if (p->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    if (index >= p->length) ER(OJE_BADINDEX);

    v = p->cards[index];
    if (p->pflags & OJF_UNIQUE) CLEAR(p->mask, v);
    --p->length;

    memmove(p->cards + index, p->cards + index + 1,
        (p->length - index) * sizeof(oj_card));
    p->eflags = 0;
    return v;
}

// Remove the given card if found.
oj_card ojl_delete_card(oj_cardlist *p, oj_card card) {
    int i;
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss);
    assert(card > 0 && card <= 54);

    if (p->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    i = ojl_index(p, card);
    if (OJE_NOTFOUND == i) ER(OJE_NOTFOUND);
    return ojl_delete(p, i);
}

// Add cards from one list to the end of another.
int ojl_extend(oj_cardlist *destp, oj_cardlist *srcp, int count) {
    assert(0 != srcp && 0x10ACE0FF == srcp->_johnnymoss);
    assert(0 != destp && 0x10ACE0FF == destp->_johnnymoss);

    if (0 == count) count = srcp->length;
    else if (count > srcp->length) count = srcp->length;
    if (0 == count) return 0;

    if (destp->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    if (count > (destp->allocation - destp->length)) ER(OJE_FULL);

    if (destp->pflags & OJF_UNIQUE) {
        int dup = 0;
        for (int i = 0; i < count; ++i) {
            if (ISSET(destp->mask, srcp->cards[i])) {
                dup = 1;
                break;
            }
        }
        if (dup) ER(OJE_DUPLICATE);
        for (int i = 0; i < count; ++i) {
            SET(destp->mask, srcp->cards[i]);
        }
    }
    memmove(destp->cards + destp->length, srcp->cards, count * sizeof(oj_card));
    destp->length += count;
    srcp->eflags = 0;
    return count;
}

// Add cards from a string to the list
int ojl_extend_text(oj_cardlist *p, char *src, int size) {
    int count, dup = 0;
    assert(0 != p && 0 != src && 0x10ACE0FF == p->_johnnymoss);

    if (0 == size) size = p->allocation - p->length;
    if (p->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    if (size > (p->allocation - p->length)) ER(OJE_FULL);
    count = ojt_vals(src, p->cards + p->length, size);

    if (p->pflags & OJF_UNIQUE) {
        for (int i = 0; i < count; ++i) {
            if (ISSET(p->mask, p->cards[i + p->length])) {
                dup = 1;
                break;
            }
        }
        if (dup) ER(OJE_DUPLICATE);
        for (int i = 0; i < count; ++i) {
            SET(p->mask, p->cards[i + p->length]);
        }
    }
    p->length += count;
    p->eflags = 0;
    return count;
}

// Copy one whole list onto another, overwriting it.
int ojl_copy(oj_cardlist *destp, oj_cardlist *srcp) {
    assert(0 != srcp && 0x10ACE0FF == srcp->_johnnymoss);
    assert(0 != destp && 0x10ACE0FF == destp->_johnnymoss);

    if (destp->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    if (destp->allocation < srcp->length) ER(OJE_FULL);

    memmove(destp->cards, srcp->cards, srcp->length * sizeof(oj_card));
    destp->length = srcp->length;
    destp->mask = srcp->mask;
    destp->eflags = 0;
    return destp->length;
}

// Begin sorting code
#define SWAP(a,b) do{t=cp[a];cp[a]=cp[b];cp[b]=t;}while(0)
#define CMP(a,b) (cp[a]>cp[b])
#define CSWP(a,b) do{s=cp[a]+cp[b];d=abs(cp[a]-cp[b]);cp[a]=(s-d)>>1;cp[b]=(s+d)>>1;}while(0)

static inline void heapify(oj_card *cp, int n, int start) {
    oj_card t;
    int lc, rc, head;
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

// Many applications involve sorting small hands inside a loop, so we go to
// some effort here to optimize the hell out of those special cases.
void _ojl_sort_cards(oj_card *cp, int n) {
    oj_card t;
    int i, s, d;

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
        // Fall back to normal in-place heapsort
    }
    for (i = (n - 1) >> 1; i >= 0; --i) heapify(cp, n - 1, i);
    for (i = n - 1; i > 0; --i) {
        SWAP(0, i);
        heapify(cp, i - 1, 0);
    }
}

// Sort cards in ascending order.
int ojl_sort(oj_cardlist *p) {
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss);
    if (p->pflags & OJF_RDONLY) ER(OJE_RDONLY);

    _ojl_sort_cards(p->cards, p->length);
    p->eflags = OJF_SORTED;
    return 0;
}

// Reverse order of cards.
int ojl_reverse(oj_cardlist *p) {
    oj_card t, *cp = p->cards;
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss);
    if (p->pflags & OJF_RDONLY) ER(OJE_RDONLY);

    for (int i = 0; i < (p->length >> 1); ++i) {
        SWAP(i, (p->length - 1) - i);
    }
    p->eflags = 0;
    return 0;
}

// Fill a sequence with fresh cards based on deck type. Can be used to fill
// multi-deck shoes as well. Removes any existing contents.
int ojl_fill(oj_cardlist *p, int count, oj_decktype dt) {
    oj_cardlist *dp = ojd_deck(dt);
    int c, remaining;
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss);

    if (p->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    if (count > p->allocation) ER(OJE_FULL);
    if ((p->pflags & OJF_UNIQUE) && (count > dp->length)) ER(OJE_DUPLICATE);

    remaining = count;
    p->length = 0;
    do {
        c = remaining;
        if (c > dp->length) c = dp->length;
        memmove(p->cards + p->length, dp->cards, c * sizeof(oj_card));

        p->length += c;
        remaining -= c;
    } while (remaining);

    if (p->pflags & OJF_UNIQUE) _build_mask(p, &p->mask);
    p->eflags = 0;
    return p->length;
}

// Standard Fisher-Yates shuffle.
int ojl_shuffle(oj_cardlist *p) {
    oj_card t, *cp = p->cards;
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss);
    if (p->pflags & OJF_RDONLY) ER(OJE_RDONLY);

    if (p->length < 2) return 0;
    for (int i = p->length-1; i > 0; --i) {
        int j = ojr_rand(i+1);
        SWAP(i,j);
    }
    p->eflags = 0;
    return 0;
}

// Fill with shuffled copy of standard deck.
int ojl_fill_shuffled(oj_cardlist *p, oj_decktype dt) {
    oj_cardlist *dp = ojd_deck(dt);
    oj_card *cp = p->cards, *sp = dp->cards;
    assert(0 != p && 0x10ACE0FF == p->_johnnymoss);

    if (p->pflags & OJF_RDONLY) ER(OJE_RDONLY);
    if (p->allocation < dp->length) ER(OJE_FULL);

    *cp = *sp;
    for (int i = 1; i < dp->length; ++i) {
        int j = ojr_rand(i+1);
        cp[i] = cp[j];
        cp[j] = sp[i];
    }
    p->length = dp->length;
    p->eflags = 0;
    return p->length;
}

// Put a string representation of the card sequence into the given buffer, clipping if
// necessary. Return the buffer, or NULL if it was too small for anything useful.
static int _minsize[] = { 3, 5, 8 };

char *ojl_text(oj_cardlist *p, char *buf, int bsize) {
    int i, sl = p->length, last = 0, clipat = sl + 1;
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
        cn = ojt_card(p->cards[i]);
        *bp++ = *cn++;
        *bp++ = *cn;
    }
    *bp++ = ')';
    *bp = 0;
    assert((bp - buf) < bsize);
    return buf;
}
