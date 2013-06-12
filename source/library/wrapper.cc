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

void Card::namesOf(std::string &s, std::vector<int> v) {
    int sz = v.size();
    char *text = new char[3 * sz + 1];

    for (int i = 0; i < sz; ++i) {
        strcpy(text + 3 * i, ojt_name(v[i]));
        if (i != sz-1) text[3 * i + 2] = ' ';
    }
    s.assign(text);
    delete[] text;
}

void Card::valuesOf(std::vector<int> &v, std::string s) {
    int max = s.size() / 2 + 1;
    int *buf = new int[max];

    int count = ojt_vals(s.c_str(), buf, max);
    v.assign(buf, buf + count);
    delete[] buf;
}

CardList::CardList(int size) {
    this->cl = new oj_cardlist_t;
    this->buffer = new int[size];
    ojl_new(this->cl, this->buffer, size);
}

CardList::CardList(int size, const char *s) {
    std::vector<int> v;
    std::string st(s);

    this->cl = new oj_cardlist_t;
    this->buffer = new int[size];
    ojl_new(this->cl, this->buffer, size);

    Card::valuesOf(v, st);
    memcpy(this->buffer, v.data(), v.size() * sizeof(int));
}

CardList::~CardList(void) {
    delete[] this->buffer;
    delete this->cl;
}

int CardList::append(const char *s) {
    std::vector<int> v;
    std::string st(s);
    Card::valuesOf(v, st);

    int r;
    for (int i = 0; i < v.size(); ++i) {
        r = ojl_append(this->cl, v[i]);
    }
    return r;
}

void CardList::text(std::string &s) {
    int size = 3 * this->cl->length + 2;
    char *buf = new char[size];
    ojl_text(this->cl, buf, size);
    s.assign(buf);
    delete[] buf;
}

} /* namespace */
