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

__attribute__((hot))
int ojb_total(const oj_cardlist_t * const sp) {
    int i, c, t = 0, ace = 0, soft = 0;
    assert(0x10ACE0FF == sp->_johnnymoss);

    for (i = 0; i < sp->length; ++i) {
        c = sp->cards[i];
        if (c >= OJ_CARD(OJR_ACE, OJS_CLUB)) {
            ace = 1;
            ++t;
        } else if (c >= OJ_CARD(OJR_TEN, OJS_CLUB)) {
            t += 10;
        } else {
            t += OJ_RANK(c) + 2;
        }
    }
    if (ace && t < 12) {
        t += 10;
        soft = 1;
    }
    return soft ? -t : t;
}
