/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "ojcardlib.h"

oj_sequence_t gdeck;
int gdbuf[60];

int do_text() {
    int i, sl, bl, hbuf[30];
    char sbuf[60], *s;
    oj_sequence_t hand;

    sl = 1 + ojr_rand(10) + ojr_rand(10) + ojr_rand(10);
    bl = 1 + ojr_rand(20) + ojr_rand(20) + ojr_rand(20);
    ojs_new(&hand, sl, hbuf);

    for (i = 0; i < sl; ++i) ojs_append(&hand, i + 1);
    s = ojs_text(&hand, sbuf, bl);
    printf("%2d %2d :%-*s:\n", sl, bl, bl-1, s);

    return 0;
}

int main(int argc, char *argv[]) {
    int i;

    ojs_new(&gdeck, 60, gdbuf);
    ojs_fill(&gdeck, 52, oj_dt_standard);

    for (i = 0; i < 1000; ++i) {
        do_text();
    }
    return EXIT_SUCCESS;
}
