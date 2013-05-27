#!/usr/bin/env python3
# OneJoker library <http://lcrocker.github.io/OneJoker>
#
# "Hello World" app included here to keep docs in sync with code.

import os, sys
sys.path.insert(1, os.path.dirname(__file__))
import ojcardlib as oj

if __name__ == "__main__":
    deck = oj.CardList(52)
    deck.fill()
    deck.shuffle()

    hand = oj.CardList(5)
    for i in range(5):
        hand.append(deck.pop())
    print(hand)
