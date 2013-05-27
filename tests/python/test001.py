#!/usr/bin/env python3
# OneJoker library <http://lcrocker.github.io/OneJoker>
#

import sys
if sys.version < "3.0":
    print("Python 3 required.")
    sys.exit(1)

sys.path.append(".")
import ojcardlib.core as oj, ojcardlib.text as ot

def progress(count):
    if (0 == (count & 0x3FFF)):
        print("\r{0:8d} remaining.".format(count), file = sys.stderr,
            end = "")

class App(object):
    def __init__(self):
        pass

    def preflop_compare(self):
        hands = [
            oj.Sequence(7, "Ac Kh"),
            oj.Sequence(7, "2s 2d"),
            oj.Sequence(7, "Ts Js")
        ]

        wins = [ 0 for h in hands ]
        values = [0 for h in hands ]
        ties = 0

        deck = oj.Sequence(52)
        deck.fill()
        for h in hands:
            deck.remove(h)

        count = 100000
        boards = oj.Iterator(deck, 5)
        print("{0:8d} total boards.".format(boards.total))

        for b in boards.random(count):
            progress(boards.remaining)

            for i, h in enumerate(hands):
                h.append(b)
                values[i], h5 = oj.poker_best5(h)
                h.truncate(2)

            best = min(values)
            if all( v == best for v in values ):
                ties += 1
            else:
                for i, v in enumerate(values):
                    if v == best:
                        wins[i] += 1
        print("\r")

        for i, h in enumerate(hands):
            print("{0} {1:8d} wins ({2:5.2f}%)".format(hands[i], wins[i], (wins[i] * 100.0) / count))

        print("        {0:8d} ties ({1:5.2f}%)".format(ties, (ties * 100.0) / count))

    def allhands(self):
        deck = oj.Sequence(52)
        deck.fill()

        hands = oj.Iterator(deck, 5)
        for h in hands.all():
            v = oj.poker_eval(h)

        print("Evaluated {0} hands.".format(hands.total))

    def list_functions(self):
        s1 = oj.Sequence(10, "2h 3s")
        s2 = oj.Sequence(10, "10s Jc")

        print(s1, s2)
        s1.append("5d")
        print(s1)
        s1.append(20)
        print(s1)
        s1.append("Kc As")
        print(s1)
        s1.append(s2)
        print(s1)

        print(s1[3], s1[6], ot.cardname(s1[2]), ot.cardname(s1[5]))
        s1[3] = 21
        # s1[2] = "5h"
        print(s1[3], s1[6], ot.cardname(s1[2]), ot.cardname(s1[5]))

        for c in (14, 15, "5d", "5h"):
            print("{0}{1}in {2}".format(c, " " if c in s1 else " not ", s1))

        s1.delete(2)
        print(s1)
        s1.remove(21)
        print(s1)
        s1.remove("Ts")
        print(s1)
        s1.remove(51)
        print(s1)

        if s1 == "2h3sKcAsJc":
            print("== 2h3sKcAsJc");
        s2 = oj.Sequence(20, "2h3sKcAsJc")
        if s1 == s2:
            print("== 2h3sKcAsJc");
        if s1 != "Kc2h3sJcAs":
            print("!= Kc2h3sJcAs")
        if s1 == (3, 8, 45, 52, 37):
            print("== 3, 8, 45, 52, 37")
        s2 = oj.Sequence( 20, (3, 8, 45, 52, 37, 12) )
        s2.pop()
        if s1 == s2:
            print("== 3, 8, 45, 52, 37")
        if s1 != (7, 12, 3, 33, 21):
            print("!= 7, 12, 3, 33, 21")

        s1.insert(0, 50)
        print(s1)
        s1.insert(3, "Qh")
        print(s1)
        s1.insert(1, s2)
        print(s1)

    def run(self):
        self.preflop_compare()
        # self.list_functions()

if __name__ == "__main__":
    App().run()
    sys.exit(0)
