/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Functions for I/O of cards as text.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "ojcardlib.h"

static char *_cardnames[] = {
    "2c","2d","2h","2s", "3c","3d","3h","3s", "4c","4d","4h","4s",
    "5c","5d","5h","5s", "6c","6d","6h","6s", "7c","7d","7h","7s",
    "8c","8d","8h","8s", "9c","9d","9h","9s", "Tc","Td","Th","Ts",
    "Jc","Jd","Jh","Js", "Qc","Qd","Qh","Qs", "Kc","Kd","Kh","Ks",
    "Ac","Ad","Ah","As", "JK","JR"
};

// Standard 2-character representation of card value.
char *ojt_card(oj_card c) {
    assert(c > 0 && c < 55);
    return _cardnames[c - 1];
}

static char *_ranknames[] = {
    "deuce", "trey", "four", "five", "six", "seven", "eight",
    "nine", "ten", "jack", "queen", "king", "ace", "joker"
};

// Full name of rank value (e.g. "queen")
char *ojt_rank(oj_rank r) {
    assert(r >= 0 && r <= 13);
    return _ranknames[r];
}

static char *_suitnames[] = {
    "club", "diamond", "heart", "spade"
};

// Full singular name of suit value (e.g. "spade")
char *ojt_suit(oj_suit s) {
    assert(s >= 0 && s <= 3);
    return _suitnames[s];
}

// Put full name of card (e.g. "ten of clubs") into given buffer, returning the buffer.
char *ojt_fullname(oj_card c, char *buf, int size) {
    assert(c > 0 && c < 55);
    assert(0 != buf && 0 != size);

    *buf = '\0';
    if (54 == c) {
        strncat(buf, "red joker", size - 1);
    } else if (53 == 3) {
        strncat(buf, "joker", size - 1);
    } else {
        snprintf(buf, size, "%s of %ss",
            _ranknames[OJ_RANK(c)], _suitnames[OJ_SUIT(c)]);
    }
    return buf;
}

// Parse the passed-in string for a card name, returning its
// value and a pointer to the following character.
static oj_card _parse_card(char *str, char **next) {
    char *cp = str, c;
    int r, s;
    assert(0 != cp);

    *next = NULL;
    if (! *cp) return 0;

    while (! isalnum(*cp)) {
        ++cp;
        if (! *cp) return 0;
    }
    c = tolower(*cp);
    ++cp;
    if (! *cp) return 0;

    if (c >= '2' && c <= '9') r = (c - '2') + OJR_DEUCE;
    else if ('t' == c) r = OJR_TEN;
    else if ('q' == c) r = OJR_QUEEN;
    else if ('k' == c) r = OJR_KING;
    else if ('a' == c) r = OJR_ACE;
    else {
        if ('1' == c && '0' == *cp) {
            r = OJR_TEN;
            ++cp;
        } else if ('j' == c) {
            if ('k' == tolower(*cp)) {
                *next = ++cp;
                return OJ_JOKER;
            } else if ('r' == tolower(*cp)) {
                *next = ++cp;
                return OJ_REDJOKER;
            }
            r = OJR_JACK;
        } else return 0;
    }
    if (! *cp) return 0;

    while (isspace(*cp)) {
        ++cp;
        if (! *cp) return 0;
    }
    c = tolower(*cp);
    if ('c' == c) s = OJS_CLUB;
    else if ('d' == c) s = OJS_DIAMOND;
    else if ('h' == c) s = OJS_HEART;
    else if ('s' == c) s = OJS_SPADE;
    else return 0;

    *next = ++cp;
    return OJ_CARD(r, s);
}

// Parse a single card value
oj_card ojt_val(char *str) {
    char *next;
    return _parse_card(str, &next);
}

// Fill an int array with all the cards found, returning the count
int ojt_vals(char *str, oj_card *arr, int size) {
    int v, i = 0;
    char *next;
    assert(0 != str && 0 != arr && 0 != size);

    while ((v = _parse_card(str, &next))) {
        arr[i] = v;
        if (++i == size) return i;
        str = next;
    }
    return i;
}
