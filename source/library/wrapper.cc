/* OneJoker RNG library <http://lcrocker.github.io/onejoker/randlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * C++ Wrapper.
 */

#include <cstdlib>
#include <cstring>
#include <assert.h>
#include "ojcardlib.h"

namespace oj {

Card::Card(oj_card v) {
    assert(v >= 0 && v <= 54);
    mV = v;
}

Card::Card(oj_rank r, oj_suit s) {
    assert(r >= 0 && r <= 14 && s >= 0 && s <= 3);
    mV = OJ_CARD(r,s);
}

Card::Card(char *s) { mV = ojt_val(s); }

oj_card Card::value(void) const { return mV; }
oj_rank Card::rank(void) const { return OJ_RANK(mV); }
oj_suit Card::suit(void) const { return OJ_SUIT(mV); }

char *Card::name(void) { return ojt_card(mV); }
void Card::fullName(std::string& s) { Card::fullNameOf(s, mV); }

int Card::equals(Card c) { return mV == c.mV; }
int Card::equals(oj_card v) { return mV == v; }
int Card::equals(char *s) { return mV == ojt_val(s); }

//static oj_card valueOf(char *s) { return ojt_val(s); }
//static char *nameOf(oj_card c) { return ojt_card(c); }

int Card::valuesOf(std::vector<oj_card> &v, std::string& s) {
    int max = (int)s.size() / 2 + 1;
    oj_card *buf = new oj_card[max];

    int count = ojt_vals(s.c_str(), buf, max);
    v.assign(buf, buf + count);
    delete[] buf;
	return count;
}

void Card::fullNameOf(std::string &s, oj_card c) {
    char buf[50];
    ojt_fullname(c, buf, sizeof(buf));
    s.assign(buf);
}

void Card::namesOf(std::string &s, const std::vector<oj_card>& v) {
    long sz = v.size();
    char *text = new char[3 * sz + 1];

    for (int i = 0; i < sz; ++i) {
        strcpy(text + 3 * i, ojt_card(v[i]));
        if (i != sz-1) text[3 * i + 2] = ' ';
    }
    s.assign(text);
    delete[] text;
}

CardList::CardList(int size) { CardList::_init(size); }

CardList::CardList(int size, const char *s) {
    CardList::_init(size);

    oj_card* cards = new oj_card[size];
    int count = ojt_vals(s, cards, size);
    memcpy(mBuf, cards, count * sizeof(oj_card));
    mL->length = count;
}

CardList::CardList(CardList *src) {
    CardList::_init(size());
    copy(src);
}

CardList::~CardList(void) {
    delete[] mBuf;
    delete mL;
}

int CardList::getPFlag(int m) { return ojl_pflag(mL, m); }
int CardList::setPFlag(int m) { return ojl_set_pflag(mL, m); }

oj_card CardList::get(int i) { return ojl_get(mL, i); }
Card CardList::getCard(int i) { return Card(ojl_get(mL, i)); }

oj_card CardList::set(int i, oj_card c) { return ojl_set(mL, i, c); }
oj_card CardList::set(int i, const Card& c) { return ojl_set(mL, i, c.value()); }
oj_card CardList::set(int i, char *s) { return ojl_set(mL, i, ojt_val(s)); }

int CardList::size(void) { return ojl_size(mL); }
int CardList::clear(void) { return ojl_clear(mL); }
int CardList::truncate(int s) { return ojl_truncate(mL, s); }

uint32_t CardList::hash(void) { return ojl_hash(mL); }
int CardList::equals(const CardList *cmp) { return ojl_equal(mL, cmp->mL); }
// TODO: (JP) this one is a little funky.
int CardList::equals(const char *s) { assert( false ); CardList temp(0, s); return equals(&temp); }

oj_card CardList::append(oj_card c) { return ojl_append(mL, c); }
oj_card CardList::append(const Card& c) { return ojl_append(mL, c.value() ); }
oj_card CardList::append(const char *s) { CardList temp(0, s); return extend( &temp ); }

oj_card CardList::pop(void) { return ojl_pop(mL); }
Card CardList::popCard(void) { return Card(ojl_pop(mL)); }
oj_card CardList::popRandom(void) { return ojl_pop_random(mL); }
Card CardList::popRandomCard(void) { return Card(ojl_pop_random(mL)); }

int CardList::index(oj_card c) { return ojl_index(mL, c); }
int CardList::index(Card c) { return ojl_index(mL, c.value()); }
int CardList::insert(int i, oj_card c) { return ojl_insert(mL, i, c); }
int CardList::insert(int i, const Card& c) { return ojl_insert(mL, i, c.value()); }

oj_card CardList::deleteIndex(int i) { return ojl_delete(mL, i); }
oj_card CardList::deleteCard(oj_card c) { return ojl_delete_card(mL, c); }
oj_card CardList::deleteCard(const Card& c) { return ojl_delete_card(mL, c.value() ); }

int CardList::extend(CardList *cl, int c) { return ojl_extend(mL, cl->mL, c); }
int CardList::extend(CardList *cl) { return ojl_extend(mL, cl->mL, 0); }
int CardList::copy(CardList *cl) { return ojl_copy(mL, cl->mL); }

int CardList::sort(void) { return ojl_sort(mL); }
int CardList::reverse(void) { return ojl_reverse(mL); }

int CardList::fill(int c, oj_decktype dt) { return ojl_fill(mL, c, dt); }
int CardList::fill(int c) { return ojl_fill(mL, c, OJD_STANDARD); }
int CardList::fill(oj_decktype dt) { return ojl_fill(mL, ojd_size(dt), dt); }
int CardList::fill(void) { return ojl_fill(mL, 52, OJD_STANDARD); }

int CardList::shuffle(void) { return ojl_shuffle(mL); }
int CardList::fill_shuffled(oj_decktype dt) { return ojl_fill_shuffled(mL, dt); }
int CardList::fill_shuffled(void) { return ojl_fill_shuffled(mL, OJD_STANDARD); }

void CardList::text(std::string &s) {
    Card::namesOf(s, std::vector<oj_card>(mBuf, mBuf + mL->length));
}

CardList CardList::newDeck(int count) {
	assert( false );
	return CardList( count );
}

} /* namespace */
