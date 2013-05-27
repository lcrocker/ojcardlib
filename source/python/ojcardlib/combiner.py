# OneJoker library <http://lcrocker.github.io/onejoker/cardlib>
#
# To the extent possibile under law, Lee Daniel Crocker has waived all
# copyright and related or neighboring rights to this work.
# <http://creativecommons.org/publicdomain/zero/1.0/>

import sys, itertools as it
from ctypes import *
from ctypes.util import find_library
ojlib = CDLL(find_library("ojcard"))
from .cardlist import CardList

class Combiner(Structure):
    _fields_ = [
        ("_johnnymoss", c_int),
        ("k", c_int),
        ("flags", c_int),
        ("map", c_int * 56),
        ("deck_invert", c_int * 56),
        ("total", c_longlong),
        ("rank", c_longlong),
        ("remaining", c_longlong),
        ("deck", POINTER(CardList)),
        ("hand", POINTER(CardList)),
        ("extra", c_void_p),
        ("filler", 4 * c_void_p),
    ]

    def __init__(self, deck, k):
        self.h = CardList(k)
        ojlib.ojc_new(byref(self), byref(deck), byref(self.h), c_longlong(0))

    def all(self):
        while ojlib.ojc_next(byref(self)):
            yield self.hand.contents

    def random(self, count):
        self.remaining = count
        while ojlib.ojc_next_random(byref(self)):
            yield self.hand.contents

    def rank(self, hand):
        if isinstance(hand, str):
            hand = CardList(self.k, hand)
        return ojlib.ojc_rank(byref(hand), byref(self))

    def hand_at(self, rank):
        h = CardList(self.k)
        ojlib.ojc_hand_at(c_longlong(rank), byref(h), byref(self))
        return h
