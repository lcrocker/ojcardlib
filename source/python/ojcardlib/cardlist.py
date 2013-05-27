# OneJoker library <http://lcrocker.github.io/onejoker/cardlib>
#
# To the extent possibile under law, Lee Daniel Crocker has waived all
# copyright and related or neighboring rights to this work.
# <http://creativecommons.org/publicdomain/zero/1.0/>

import sys, itertools as it
from ctypes import *
from ctypes.util import find_library
ojlib = CDLL(find_library("ojcard"))
from . import text

class ReadOnlyError(Exception):
    pass
class ListFullError(Exception):
    pass
class DuplicateError(Exception):
    pass

def ojerror(e):
    if (e >= 0): return e
    if -2 == e: raise ReadOnlyError()
    if -3 == e: raise ListFullError()
    if -4 == e: raise DuplicateError()
    if -5 == e: raise IndexError
    assert(False)

class CardList(Structure):
    _fields_ = [
        ("_johnnymoss", c_int),
        ("allocation", c_int),
        ("length", c_int),
        ("pflags", c_int),
        ("eflags", c_int),
        ("mask", c_longlong),
        ("cards", POINTER(c_int)),
        ("extra", c_void_p),
        ("filler", 6 * c_void_p),
    ]

    def __init__(self, allocation, init = None):
        self.buf = cast(create_string_buffer(4 * allocation), POINTER(c_int))
        ojlib.ojl_new(byref(self), self.buf, allocation)
        if init:
            self.append(init)

    def __len__(self):
        return self.length

    def __iter__(self):
        for i in range(self.length):
            yield self.cards[i]

    def __str__(self):
        return "({0})".format(text.cardnames(
            (self.cards[i] for i in range(self.length)), " "))

    def __eq__(self, other):
        if isinstance(other, CardList):
            return ojlib.ojl_equal(byref(self), byref(other))
        else:
            if isinstance(other, str):
                other = text.cardnums(other)
            return all( a == b for a, b in it.zip_longest(other,
                (self.cards[i] for i in range(self.length)), fillvalue = 0) )

    def __getitem__(self, index):
        if index >= self.length:
            raise IndexError
        return self.cards[index]

    def __setitem__(self, index, val):
        if index >= self.length:
            raise IndexError
        if isinstance(val, str):
            val = cardnum(val)
        ojerror(ojlib.ojl_set(byref(self), val))

    def __contains__(self, val):
        if isinstance(val, str):
            val = text.cardnum(val)
        return (-1 != ojlib.ojl_index(byref(self), val))

    def clear(self):
        ojerror(ojlib.ojl_clear(byref(self)))

    def delete(self, index):
        return ojerror(ojlib.ojl_delete(byref(self), index))

    def truncate(self, length):
        return ojerror(ojlib.ojl_truncate(byref(self), length))

    def append(self, arg):
        if isinstance(arg, str):
            for c in text.cardnums(arg):
                ojerror(ojlib.ojl_append(byref(self), c))
        elif hasattr(arg, "__iter__"):
            for v in arg:
                ojerror(ojlib.ojl_append(byref(self), v))
        else:
            ojerror(ojlib.ojl_append(byref(self), arg))

    def insert(self, index, arg):
        if isinstance(arg, str):
            for c in text.cardnums(arg):
                ojerror(ojlib.ojl_insert(byref(self), index, c))
                index += 1
        elif hasattr(arg, "__iter__"):
            for v in arg:
                ojerror(ojlib.ojl_insert(byref(self), index, v))
                index += 1
        else:
            ojerror(ojlib.ojl_insert(byref(self), index, arg))

    def remove(self, arg):
        if isinstance(arg, str):
            for c in text.cardnums(arg):
                v = ojerror(ojlib.ojl_remove(byref(self), c))
            return v
        elif hasattr(arg, "__iter__"):
            for v in arg:
                v = ojerror(ojlib.ojl_remove(byref(self), v))
            return v
        else:
            return ojerror(ojlib.ojl_remove(byref(self), arg))

    def pop(self):
        return ojerror(ojlib.ojl_pop(byref(self)))

    def index(self, card):
        if isinstance(card, str):
            card = text.cardnum(card)
        return ojlib.ojl_index(byref(self), card)

    def fill(self, count = 52, type = None):
        if type is None:
            type = 0
        ojerror(ojlib.ojl_fill(byref(self), count, type))

    def sort(self):
        ojerror(ojlib.ojl_sort(byref(self)))

    def reverse(self):
        ojerror(ojlib.ojl_reverse(byref(self)))

    def shuffle(self):
        ojerror(ojlib.ojl_shuffle(byref(self)))
