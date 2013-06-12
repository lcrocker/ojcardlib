/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 */

#ifndef _OJCARDLIB_H
#define _OJCARDLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* TYPES */

typedef int_fast8_t oj_card;

typedef enum _oj_rank {
    OJR_DEUCE = 0,  OJR_TWO = 0,
    OJR_TREY = 1,   OJR_THREE = 1,
    OJR_FOUR = 2,
    OJR_FIVE = 3,
    OJR_SIX = 4,
    OJR_SEVEN = 5,
    OJR_EIGHT = 6,
    OJR_NINE = 7,
    OJR_TEN = 8,
    OJR_JACK = 9,   OJR_KNAVE = 9,
    OJR_QUEEN = 10,
    OJR_KING = 11,
    OJR_ACE = 12,
    OJR_JOKER = 13
} oj_rank;

typedef enum _oj_suit {
    OJS_CLUB = 0, OJS_DIAMOND = 1, OJS_HEART = 2, OJS_SPADE = 3
} oj_suit;

typedef enum _oj_decktype {
    OJD_STANDARD = 0,
    OJD_ONEJOKER = 1,
    OJD_TWOJOKERS = 2,
    OJD_SKAT = 3,
    OJD_PAN = 4,
    OJD_PANJ = 5,
    OJD_PINOCHLE = 6,
} oj_decktype;

typedef struct _oj_cardlist {
    int _johnnymoss;
    int_fast16_t allocation, length;
    int_fast16_t pflags, eflags;
    uint64_t mask;
    oj_card *cards;
    void *filler[6];
} oj_cardlist;

// Flag masks
#define OJF_RDONLY 1
#define OJF_UNIQUE 2
#define OJF_SORTED 1

typedef struct _oj_combiner {
    int _johnnymoss;
    int_fast16_t k, flags;
    oj_cardlist *deck, *hand;
    oj_card map[56], invert[56];
    int64_t total, remaining;
    void *filler[4];
} oj_combiner;

typedef struct _oj_poker_hand_info {
    int _johnnymoss;
    int val, group, nranks;
    int ranks[5];
    void *filler[4];
} oj_poker_hand_info;


/* GLOBALS */


/* CONSTANTS */

#define OJ_JOKER 53
#define OJ_BLACKJOKER 53
#define OJ_REDJOKER 54

#define OJE_NOTFOUND (-1)
#define OJE_RDONLY (-2)
#define OJE_FULL (-3)
#define OJE_DUPLICATE (-4)
#define OJE_BADINDEX (-5)


/* MACROS */

#define OJ_RANK(c) ((oj_rank)(((c) - 1) >> 2))
#define OJ_SUIT(c) ((oj_suit)(((c) - 1) & 3))
#define OJ_CARD(r,s) ((oj_card)((((int)(r) << 2) | (int)(s)) + 1))
#define OJL_GET(p,i) ((p)->cards[i])
#define OJL_SET(p,i,c) ((p)->cards[i]=(c))
#define OJL_CLEAR(p) ((p)->length=0)
#define OJL_TRUNCATE(p,s) (((s)<(p)->length)?((p)->length=(s)):0)
#define OJL_APPEND(p,c) (((p)->length == (p)->allocation)?0:\
((p)->cards[(p)->length++]=(c)))
#define OJL_POP(p) ((0==(p)->length)?0:(p)->cards[--(p)->length])


/* PROTOTYPES */

extern int oj_init_library(int);

// deckinfo.c
extern int ojd_ntypes(void);
extern int ojd_size(oj_decktype);
extern oj_cardlist *ojd_deck(oj_decktype);

// text.c
extern char *ojt_card(oj_card);
extern char *ojt_rank(oj_rank);
extern char *ojt_suit(oj_suit);
extern char *ojt_fullname(oj_card, char *, int);
extern oj_card ojt_val(char *str);
extern int ojt_vals(char *str, oj_card *arr, int size);

// prng.c
extern int ojr_seed(int);
extern uint16_t ojr_next16(void);
extern uint32_t ojr_next32(void);
extern uint64_t ojr_next64(void);
extern double ojr_next_double(void);
extern int ojr_rand(int);

// cardlist.c
extern int ojl_new(oj_cardlist *, oj_card *, int);
extern int ojl_pflag(oj_cardlist *, int);
extern int ojl_eflag(oj_cardlist *, int);
extern int ojl_set_pflag(oj_cardlist *, int);
extern int ojl_set_eflag(oj_cardlist *, int);
extern int ojl_clear_pflag(oj_cardlist *, int);
extern int ojl_clear_eflag(oj_cardlist *, int);
extern oj_card ojl_get(oj_cardlist *, int);
extern oj_card ojl_set(oj_cardlist *, int, oj_card);
extern int ojl_size(oj_cardlist *);
extern int ojl_clear(oj_cardlist *);
extern int ojl_truncate(oj_cardlist *, int);
extern int ojl_equal(oj_cardlist *, oj_cardlist *);
extern uint32_t ojl_hash(oj_cardlist *);
extern oj_card ojl_append(oj_cardlist *, oj_card);
extern oj_card ojl_pop(oj_cardlist *);
extern oj_card ojl_pop_random(oj_cardlist *);
extern int ojl_index(oj_cardlist *, oj_card);
extern int ojl_insert(oj_cardlist *, int, oj_card);
extern oj_card ojl_delete(oj_cardlist *, int);
extern oj_card ojl_delete_card(oj_cardlist *, oj_card);
extern int ojl_extend(oj_cardlist *, oj_cardlist *, int);
extern int ojl_extend_text(oj_cardlist *, char *, int);
extern int ojl_copy(oj_cardlist *, oj_cardlist *);
extern int ojl_sort(oj_cardlist *);
extern int ojl_reverse(oj_cardlist *);
extern int ojl_fill(oj_cardlist *, int, oj_decktype);
extern int ojl_shuffle(oj_cardlist *);
extern int ojl_fill_shuffled(oj_cardlist *, oj_decktype);
extern char *ojl_text(oj_cardlist *, char *, int);

// combiner.c
extern int64_t ojc_binomial(int, int);
extern int ojc_new(oj_combiner *, oj_cardlist *, oj_cardlist *, int, int64_t);
extern int ojc_next(oj_combiner *);
extern int ojc_next_random(oj_combiner *);
extern int64_t ojc_colex_rank(oj_combiner *, oj_cardlist *);
extern int ojc_colex_hand_at(oj_combiner *, int64_t, oj_cardlist *);

// blackjack.c
extern int ojb_total(oj_cardlist *);

// poker.c
extern int ojp_eval5(oj_cardlist *);
extern int ojp_eval7(oj_cardlist *);
extern int ojp_best5(oj_cardlist *, oj_cardlist *);
extern int ojp_hand_info(oj_poker_hand_info *, oj_cardlist *, int val);
extern char *ojp_hand_description(oj_poker_hand_info *, char *, int);


#ifdef __cplusplus
} /* end of extern "C" */
#endif /* __cplusplus */


/* C++ DECLARATIONS */

#ifdef __cplusplus
#include <vector>
#include <string>
namespace oj {

class Card {
private:
    int mValue;

public:
    Card(int v) { mValue = v; }
    Card(int r, int s) { mValue = OJ_CARD(r,s); }
    Card(const char *s) { mValue = ojt_val(s); }

    int value(void) { return mValue; }
    int rank(void) { return OJ_RANK(mValue); }
    int suit(void) { return OJ_SUIT(mValue); }
    char *name(void) { return ojt_name(mValue); }

    int equals(Card c) { return mValue == c.mValue; }
    int equals(int v) { return mValue == v; }
    int equals(const char *s) { return mValue == ojt_val(s); }

    static const char *nameOf(int c) { return ojt_name(c); }
    static void namesOf(std::string &s, std::vector<int> v);
    static int valueOf(const char *s) { return ojt_val(s); }
    static void valuesOf(std::vector<int> &v, std::string s);
};


class CardList {
private:
    oj_cardlist *cl;
    int *buffer;

public:
    CardList(int size);
    CardList(int size, const char *s);
    ~CardList(void);

    int truncate(int s) { return ojl_truncate(this->cl, s); }
    int clear(void) { return ojl_clear(this->cl); }
    int size(void) { return ojl_size(this->cl); }

    int get(int i) { return ojl_get(this->cl, i); }
    int set(int i, int c) { return ojl_set(this->cl, i, c); }
    int set(int i, const char *s) { return ojl_set(this->cl, i, ojt_val(s)); }
    int getPflag(int m) { return ojl_pflag(this->cl, m); }
    int setPflag(int m) { return ojl_set_pflag(this->cl, m); }
    uint32_t hash(void) { return ojl_hash(this->cl); }

    int append(int c) { return ojl_append(this->cl, c); }
    int append(Card c) { return ojl_append(this->cl, c.mValue); }
    int append(const char *s);
    int copy(CardList *other) { return ojl_copy(this->cl, other->cl); }

    int extend(CardList *src, int count) {
        return ojl_extend(this->cl, src->cl, count);
    }
    int extend(Cardlist *src) {
        return ojl_extend(this->cl, src->cl, src->cl->length);
    }
    int equals(Cardlist *other) { return ojl_equal(this->cl, other->cl); }

    int fill(int count, oj_decktype_t dt) {
        return ojl_fill(this->cl, count, dt);
    }
    int fill(int count) { return ojl_fill(this->cl, count, OJD_STANDARD); }
    int fill(oj_decktype_t dt) {
        return ojl_fill(this->cl, this->cl->length, dt);
    }
    int fill(void) {
        return ojl_fill(this->cl, this->cl->length, OJD_STANDARD);
    }
    int insert(int i, int c) { return ojl_insert(this->cl, i, c); }
    int insert(int i, Card c) { return ojl_insert(this->cl, i, c.mValue); }
    int index(int c) { return ojl_index(this->cl, c); }

    int pop(void) { return ojl_pop(this->cl); }
    int removeIndex(int i) { return ojl_delete(this->cl, i); }
    int removeCard(int c) { return ojl_remove(this->cl, c);
    int removeCard(Card c) { return ojl_remove(this->cl, c.mValue);
    int reverse(void) { return ojl_reverse(this->cl); }
    int sort(void) { return ojl_sort(this->cl); }
    int shuffle(void) { return ojl_shuffle(this->cl); }

    void text(std::string &s);
};


class Combiner {

};


} /* namespace oj */
#endif /* __cplusplus for class definitions */

#endif /* _OJCARDLIB_H */
