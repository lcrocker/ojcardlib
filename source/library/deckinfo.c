/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Information on types of card decks.
 */

#include <stdlib.h>
#include "ojcardlib.h"

// Standard 52-card deck
static oj_card _standard_cards[] = {
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
    25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,
    46,47,48,49,50,51,52
};

// 53-card deck with one joker
static oj_card _onejoker_cards[] = {
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
    25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,
    46,47,48,49,50,51,52,53
};

// 54 cards, one "black" joker and one "red" joker
static oj_card _twojokers_cards[] = {
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
    25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,
    46,47,48,49,50,51,52,53,54
};

// 2s through 6s removes (as in Manila poker, Skat)
static oj_card _skat_cards[] = {
    21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,
    37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52
};

// 8s, 9s, 10s removed (as in Panguingue)
static oj_card _pan_cards[] = {
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
    37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52
};

// 8s, 9s, 10s removed, joker added (as in Mexican poker)
static oj_card _panj_cards[] = {
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
    37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53
};

// 9s and above only (Pinochle)
static oj_card _pinochle_cards[] = {
    29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52
};

#define OJD_NTYPES 7

oj_cardlist _oj_common_decks[] = {
    { 0x10ACE0FF, 52, 52, OJF_RDONLY, 0, 0LL, _standard_cards, {0,0,0,0} },
    { 0x10ACE0FF, 53, 53, OJF_RDONLY, 0, 0LL, _onejoker_cards, {0,0,0,0} },
    { 0x10ACE0FF, 54, 54, OJF_RDONLY, 0, 0LL, _twojokers_cards, {0,0,0,0} },
    { 0x10ACE0FF, 32, 32, OJF_RDONLY, 0, 0LL, _skat_cards, {0,0,0,0} },
    { 0x10ACE0FF, 40, 40, OJF_RDONLY, 0, 0LL, _pan_cards, {0,0,0,0} },
    { 0x10ACE0FF, 41, 41, OJF_RDONLY, 0, 0LL, _panj_cards, {0,0,0,0} },
    { 0x10ACE0FF, 24, 24, OJF_RDONLY, 0, 0LL, _pinochle_cards, {0,0,0,0} },
};

int ojd_ntypes(void) { return OJD_NTYPES; }

int ojd_size(oj_decktype t) { return _oj_common_decks[t].length; }

oj_cardlist *ojd_deck(oj_decktype t) { return &_oj_common_decks[t]; }
