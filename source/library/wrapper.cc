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

#include "ojcardlib.h"

namespace oj {

Card::Card(int v) {
    assert(v >= 0 && v <= 54);
    mV = v;
}

Card::Card(int r, int s) {
    assert(r >= 0 && r <= 14 s >= 0 && s <= 3);
    mV = OJ_CARD(r,s);
}

Card::Card(char *) { mV = Card::valueOf(s); }

int Card::value(void) { return mV; }
int Card::rank(void) { return OJ_RANK(mV); }
int Card::suit(void) { return OJ_SUIT(mV); }

char *Card::name(void) { return Card::nameOf(mV); }
void Card::fullName(std::string s) { Card::fullnameOf(s, mV); }

int Card::equals(Card c) { return mV == c.mV; }
int Card::equals(int v) { return mV == v; }
int Card::equals(char *s) { return mV == Card::valueOf(s); }

static oj_card valueOf(char *s) { return ojt_val(s); }
static char *nameOf(oj_card c) { return ojt_card(c); }

static int Card::valuesOf(std::vector<oj_card> &v, char *s) {
    int max = s.size() / 2 + 1;
    oj_card *buf = new oj_card[max];

    int count = ojt_vals(s, buf, max);
    v.assign(buf, buf + count);
    delete[] buf;
}

static void Card::fullNameOf(std::string &s, int c) {
    char buf[50];
    ojt_fullname(c, buf, sizeof(buf));
    s.assign(buf);
}

static void Card::namesOf(std::string &s, std::vector<oj_card> v) {
    int sz = v.size();
    char *text = new char[3 * sz + 1];

    for (int i = 0; i < sz; ++i) {
        strcpy(text + 3 * i, ojt_name(v[i]));
        if (i != sz-1) text[3 * i + 2] = ' ';
    }
    s.assign(text);
    delete[] text;
}

CardList::CardList(int size) { CardList::_init(size); }

CardList::CardList(int size, char *s) {
    CardList::_init(size);

    cards = new oj_card[size];
    int count = ojt_vals(s, cards, size);
    memcpy(mBuf, cards, count * sizeof(oj_card));
    mL->length = count;
}

CardList::CardList(CardList *src) {
    CardList::_init(size);
    this.copy(src);
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
oj_card CardList::set(int i, Card c) { return ojl_set(mL, i, c.mV); }
oj_card CardList::set(int i, char *s) { return ojl_set(mL, i, ojt_val(s)); }

int CardList::size(void) { return ojt_size(mL); }
int CardList::clear(void) { return ojt_clear(mL); }
int CardList::truncate(int s) { return ojl_truncate(mL, s); }

uint32_t CardList::hash(void) { return ojl_hash(mL); }
int CardList::equals(CardList *cmp) { return ojl_equal(mL, cmp); }
int CardList::equals(char *s) { return this.equals(&CardList(s)); }

oj_card CardList::append(oj_card c) { return ojl_append(mL, c); }
oj_card CardList::append(Card c) { return ojl_append(mL, c.mV); }
oj_card CardList::append(char *s) { return this.extend(&Cardlist(s)); }

oj_card CardList::pop(void) { return ojl_pop(mL); }
Card CardList::popCard(void) { return Card(ojl_pop(mL)); }
oj_card CardList::popRandom(void) { return ojl_pop_random(mL); }
Card CardList::popRandomCard(void) { return Card(ojl_pop_random(mL)); }

int CardList::index(oj_card c) { return ojl_index(mL, c); }
int CardList::index(Card c) { return ojl_index(mL, c.mV); }
int CardList::insert(int i, oj_card c) { return ojl_insert(mL, i, c); }
int CardList::insert(int i, Card c) { return ojl_insert(mL, i, c.mV); }

oj_card CardList::deleteIndex(int i) { return ojl_delete(mL, i); }
oj_card CardList::deleteCard(oj_card c) { return ojl_delete_card(mL, c); }
oj_card CardList::deleteCard(Card c) { return ojl_delete_card(mL, c.mV); }

int CardList::extend(CardList *cl, int c) { return ojl_extend(mL, cl.mL, c); }
int CardList::extend(CardList *cl) { return ojl_extend(mL, cl.mL, 0); }
int CardList::extend(char *s) { return this.extend(&Cardlist(s)); }
int CardList::copy(CardList *cl) { return ojl_copy(mL, cl.mL); }

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
    Card::namesOf(s, std::vector<oj_card>(mBuf, mBuf + mL.length));
}

static CardList CardList::newDeck(int);

} /* namespace */
