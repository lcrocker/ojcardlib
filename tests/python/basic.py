#!/usr/bin/env python3
# OneJoker library <http://lcrocker.github.io/OneJoker>
# Basic sanity checks for Python modules.

import sys, os
if sys.version < "3.0":
    print("Python 3 required.")
    sys.exit(1)

# These tests assume that they are being run from the build
# directory, and that the modules have been built here.
sys.path.insert(1, os.path.dirname(os.path.abspath(__file__)))

import onejoker as oj

class App(object):
    def cards_and_decks(self):
        if 13 != oj.card(oj.r_five, oj.s_club): return 1
        if 30 != oj.card(oj.r_nine, oj.s_diamond): return 2
        if 43 != oj.card(oj.r_queen, oj.s_heart): return 3
        if 52 != oj.card(oj.r_ace, oj.s_spade): return 4
        if 54 != oj.c_redjoker: return 5

        if oj.r_trey != oj.rank(7): return 6
        if oj.s_diamond != oj.suit(26): return 7
        if oj.r_jack != oj.rank(38): return 8
        if oj.s_spade != oj.suit(48): return 9
        if oj.r_joker != oj.rank(54): return 10

        if 52 != oj.decksize(oj.dt_standard): return 11
        if 54 != oj.decksize(oj.dt_twojokers): return 12
        if 40 != oj.decksize(oj.dt_stripped40): return 13

        d = oj.newdeck(oj.dt_onejoker)
        if 53 != len(d): return 14
        if oj.card(oj.r_deuce, oj.s_club) != d[0]: return 15
        if oj.c_joker != d[52]: return 16

        d = oj.newdeck(oj.dt_stripped32)
        if 32 != len(d): return 17
        if oj.card(oj.r_seven, oj.s_club) != d[0]: return 18
        if oj.card(oj.r_ace, oj.s_spade) != d[31]: return 19

        d = oj.newdeck(oj.dt_stripped40j)
        if 41 != len(d): return 20
        if oj.card(oj.r_seven, oj.s_spade) != d[23]: return 21
        if oj.card(oj.r_jack, oj.s_club) != d[24]: return 22
        if oj.c_joker != d[40]: return 23

        return 0

    def text_functions(self):
        return 0

    def run(self):
        failed = self.cards_and_decks()
        failed = 100 * failed + self.text_functions()

        print("Basic Python tests ", end = "")
        if (failed):
            print("failed (code = {0}).".format(failed))
            return 1;
        else:
            print("passed.")
        return 0;

if __name__ == "__main__":
    sys.exit(App().run())
