/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Blackjack functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "ojcardlib.h"

int ojb_total(oj_cardlist *p) {
    int total = 0, ace = 0, soft = 0;
    assert(0x10ACE0FF == p->_johnnymoss);

    for (int i = 0; i < p->length; ++i) {
        int c = p->cards[i];
        if (c >= OJ_CARD(OJR_ACE, OJS_CLUB)) {
            ace = 1;
            total += 1;
        } else if (c >= OJ_CARD(OJR_TEN, OJS_CLUB)) {
            total += 10;
        } else {
            total += OJ_RANK(c) + 2;
        }
    }
    if (ace && total < 12) {
        total += 10;
        soft = 1;
    }
    return soft ? -total : total;
}
