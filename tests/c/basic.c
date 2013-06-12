/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Test card sequence and name functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "ojcardlib.h"

int cards_and_decks() {
    oj_cardlist *dp;

    if (13 != OJ_CARD(OJR_FIVE, OJS_CLUB)) return 1;
    if (30 != OJ_CARD(OJR_NINE, OJS_DIAMOND)) return 2;
    if (43 != OJ_CARD(OJR_QUEEN, OJS_HEART)) return 3;
    if (52 != OJ_CARD(OJR_ACE, OJS_SPADE)) return 4;
    if (54 != OJ_REDJOKER) return 5;

    if (OJR_TREY != OJ_RANK(7)) return 6;
    if (OJS_DIAMOND != OJ_SUIT(26)) return 7;
    if (OJR_JACK != OJ_RANK(38)) return 8;
    if (OJS_SPADE != OJ_SUIT(48)) return 9;
    if (OJR_JOKER != OJ_RANK(54)) return 10;

    if (ojd_ntypes() < 7) return 11;
    if (52 != ojd_size(OJD_STANDARD)) return 12;
    if (24 != ojd_size(OJD_PINOCHLE)) return 13;
    if (40 != ojd_size(OJD_PAN)) return 14;

    dp = ojd_deck(OJD_ONEJOKER);
    if (53 != dp->length) return 15;
    if (OJ_CARD(OJR_DEUCE, OJS_CLUB) != dp->cards[0]) return 16;
    if (OJ_JOKER != dp->cards[52]) return 17;

    dp = ojd_deck(OJD_SKAT);
    if (32 != dp->length) return 18;
    if (OJ_CARD(OJR_SEVEN, OJS_CLUB) != dp->cards[0]) return 19;
    if (OJ_CARD(OJR_ACE, OJS_SPADE) != dp->cards[31]) return 20;

    dp = ojd_deck(OJD_PANJ);
    if (41 != dp->length) return 21;
    if (OJ_CARD(OJR_SEVEN, OJS_SPADE) != dp->cards[23]) return 22;
    if (OJ_CARD(OJR_JACK, OJS_CLUB) != dp->cards[24]) return 23;
    if (OJ_JOKER != dp->cards[40]) return 24;

    return 0;
}

int text_functions() {
    int v;
    oj_card arr[20];
    char t[60];
    oj_cardlist seq;

    if (0 != strcmp("Jd",
        ojt_card(OJ_CARD(OJR_JACK, OJS_DIAMOND)))) return 1;
    if (0 != strcmp("Tc",
        ojt_card(OJ_CARD(OJR_TEN, OJS_CLUB)))) return 2;
    if (0 != strcmp("5h",
        ojt_card(OJ_CARD(OJR_FIVE, OJS_HEART)))) return 3;
    if (0 != strcmp("JK", ojt_card(OJ_JOKER))) return 4;

    if (0 != strcmp("deuce", ojt_rank(OJR_DEUCE))) return 5;
    if (0 != strcmp("ten", ojt_rank(OJR_TEN))) return 6;
    if (0 != strcmp("joker", ojt_rank(OJR_JOKER))) return 7;
    if (0 != strcmp("club", ojt_suit(OJS_CLUB))) return 8;
    if (0 != strcmp("heart", ojt_suit(OJS_HEART))) return 9;

    ojt_fullname(OJ_CARD(OJR_JACK, OJS_DIAMOND), t, 60);
    if (0 != strcmp("jack of diamonds", t)) return 10;
    ojt_fullname(OJ_CARD(OJR_NINE, OJS_CLUB), t, 60);
    if (0 != strcmp("nine of clubs", t)) return 11;
    ojt_fullname(OJ_CARD(OJR_SEVEN, OJS_SPADE), t, 60);
    if (0 != strcmp("seven of spades", t)) return 12;

    if (OJ_CARD(OJR_DEUCE, OJS_SPADE) != ojt_val("2s")) return 13;
    if (OJ_CARD(OJR_FOUR, OJS_CLUB) != ojt_val(" 4 c")) return 14;
    if (OJ_CARD(OJR_SIX, OJS_DIAMOND) != ojt_val("**6d")) return 15;
    if (OJ_CARD(OJR_QUEEN, OJS_SPADE) != ojt_val("qS")) return 16;
    if (OJ_REDJOKER != ojt_val("JR")) return 17;

    ojl_new(&seq, arr, 20);
    ojl_append(&seq, ojt_val("3s"));
    ojl_append(&seq, ojt_val("Ad"));
    ojl_append(&seq, ojt_val("Jc"));
    ojl_append(&seq, ojt_val("7d"));
    ojl_append(&seq, ojt_val("10s"));

    ojl_text(&seq, t, 60);
    if (0 != strcmp("(3s Ad Jc 7d Ts)", t)) return 18;

    v = ojt_vals("KsKc5c9d2hQcJk", arr, 19);
    if (7 != v) return 20;
    if (OJ_CARD(OJR_KING, OJS_SPADE) != arr[0]) return 21;
    if (OJ_CARD(OJR_QUEEN, OJS_CLUB) != arr[5]) return 22;

    ojl_text(&seq, t, 60);
    if (0 != strcmp("(Ks Kc 5c 9d 2h)", t)) return 23;

    return 0;
}

int main(int argc, char *argv[]) {
    int failed = 0;

    failed = cards_and_decks();
    failed = 100 * failed + text_functions();

    fprintf(stderr, "Basic C tests ");
    if (failed) {
        fprintf(stderr, "failed (code = %d).\n", failed);
    } else {
        fprintf(stderr, "passed.\n");
    }
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
