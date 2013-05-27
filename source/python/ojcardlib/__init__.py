"""OneJoker library <http://lcrocker.github.io/onejoker/cardlib>
"""
# To the extent possibile under law, Lee Daniel Crocker has waived all
# copyright and related or neighboring rights to this work.
# <http://creativecommons.org/publicdomain/zero/1.0/>

from .core import rank, suit, card, seed, rand, decksize, newdeck, \
	binomial, poker_eval5, poker_best5
from .text import cardname, cardnames, rankname, suitname, cardnum, cardnums
from .cardlist import CardList
from .combiner import Combiner

r_deuce = 0
r_trey = 1
r_four = 2
r_five = 3
r_six = 4
r_seven = 5
r_eight = 6
r_nine = 7
r_ten = 8
r_jack = 9
r_queen = 10
r_king = 11
r_ace = 12
r_joker = 13

s_club = 0
s_diamond = 1
s_heart = 2
s_spade = 3

c_joker = 53
c_redjoker = 54

dt_standard = 0
dt_onejoker = 1
dt_twojokers = 2
dt_stripped32 = 3
dt_stripped40 = 4
dt_stripped40j = 5
dt_pinochle = 6

pg_standard = 0
pg_acetofive = 1
pg_deucetoseven = 2
pg_acetosix = 3
pg_badugi = 4
