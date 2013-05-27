# OneJoker library <http://lcrocker.github.io/onejoker/cardlib>
#
# To the extent possibile under law, Lee Daniel Crocker has waived all
# copyright and related or neighboring rights to this work.
# <http://creativecommons.org/publicdomain/zero/1.0/>

import sys, itertools as it
from .cardlist import CardList
from ctypes import *
from ctypes.util import find_library

ojlib = CDLL(find_library("ojcard"))
ojlib.ojc_binomial.restype = c_longlong

def rank(c):
    return (c - 1) >> 2

def suit(c):
    return (c - 1) & 3

def card(r, s):
    return ((r << 2) | s) + 1

def seed(val):
    return ojlib.ojr_seed(val)

def rand(limit):
    return ojlib.ojr_rand(limit)

def decksize(t):
    return ojlib.ojd_size(t)

def newdeck(t = 0):
    n = ojlib.ojd_size(t)
    d = CardList(n)
    d.fill(n, t)
    return d

def binomial(n, k):
    if (n <= 54 and k <= 54):
        return ojlib.ojc_binomial(n, k)
    b = 1
    for i in range(1, k + 1):
        b *= (n - (k - i));
        b //= i
    return b

def poker_eval5(hand):
    return ojlib.ojp_eval5(byref(hand))

def poker_best5(hand):
    b = Sequence(5)
    v = ojlib.ojp_best5(byref(hand), byref(b))
    return v, b
