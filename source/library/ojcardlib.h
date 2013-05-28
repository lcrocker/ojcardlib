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


/* Different types of card decks. Have to keep the specific numbers here
 * to stay in sync with the other language bindings.
 */
typedef enum _oj_deck_type {
    OJD_STANDARD = 0,           /* Normal 52-card deck */
    OJD_ONEJOKER = 1,           /* 53 */
    OJD_TWOJOKERS = 2,          /* 54 */
    OJD_STRIPPED32 = 3,         /* Remove 2..6 (Skat) */
    OJD_STRIPPED40 = 4,         /* Remove 8,9,10 (Pan) */
    OJD_STRIPPED40J = 5,        /* (Mexican poker) */
    OJD_PINOCHLE = 6,           /* 9s and up (Pinochle) */
} oj_deck_type_t;


/* STRUCTURES */

/* Simple ordered sequence of cards.
 */
typedef struct _oj_cardlist {
    int _johnnymoss;            /* Initialization check. */
    int allocation;             /* Maximum size--fixed. */
    int length;                 /* Current content size */
    int pflags;                 /* Persistent flags */
    int eflags;                 /* Ephemeral flags */
    uint64_t mask;              /* Bitmask for unique lists */
    int *cards;                 /* Actual contents */
    void *extra;                /* Whatever the caller adds */
    void *filler[6];            /* ABI change room */
} oj_cardlist_t;

/* Flag masks
 */
#define OJF_RDONLY 1
#define OJF_UNIQUE 2
#define OJF_SORTED 1

/* Tracking combinations of <k> cards from set of <n>.
 */
typedef struct _oj_combiner {
    int _johnnymoss;            /* Initialization check */
    int k;                      /* Combination size */
    int flags;                  /* General purpose */
    int map[56];                /* 0-based indices into deck */
    int deck_invert[56];        /* Inverse array used for ranking */
    long long total;            /* Total combinations */
    long long rank;             /* Colex rank of current comb. */
    long long remaining;        /* Countdown */
    oj_cardlist_t *deck;        /* Universe of combinations */
    oj_cardlist_t *hand;        /* Current combination */
    void *extra;                /* Whatever caller adds */
    void *filler[4];            /* ABI change room */
} oj_combiner_t;

/* Flag masks
 */
#define OJF_VALIDMAP 1
#define OJF_VALIDRANK 2

/* Information needed to display poker hands to humans in nice ways.
 */
typedef struct _oj_poker_hand_info {
    int _johnnymoss;
    int val;                    /* Equiv. class rank */
    int group;                  /* SF, Quads, etc. */
    int nranks;                 /* # of significant ranks within group */
    int ranks[5];               /* The ranks */
    void *filler[4];
} oj_poker_hand_info_t;


/* GLOBALS */


/* CONSTANTS */

#define OJR_DEUCE 0
#define OJR_TWO 0
#define OJR_TREY 1
#define OJR_THREE 1
#define OJR_FOUR 2
#define OJR_FIVE 3
#define OJR_SIX 4
#define OJR_SEVEN 5
#define OJR_EIGHT 6
#define OJR_NINE 7
#define OJR_TEN 8
#define OJR_JACK 9
#define OJR_QUEEN 10
#define OJR_KING 11
#define OJR_ACE 12
#define OJR_JOKER 13

#define OJS_CLUB 0
#define OJS_DIAMOND 1
#define OJS_HEART 2
#define OJS_SPADE 3

#define OJ_JOKER 53
#define OJ_BLACKJOKER 53
#define OJ_REDJOKER 54

/* Error codes */
#define OJE_NOTFOUND (-1)
#define OJE_RDONLY (-2)
#define OJE_FULL (-3)
#define OJE_DUPLICATE (-4)
#define OJE_BADINDEX (-5)


/* MACROS */

/* Get rank, suit value from card */
#define OJ_RANK(c) (((c) - 1) >> 2)
#define OJ_SUIT(c) (((c) - 1) & 3)

/* Build card value from rank, suit */
#define OJ_CARD(r,s) ((((r) << 2) | (s)) + 1)

/* Fast macro versions of a few cardlist functions. These don't respect flags
 * or do other error checking that the functions do, so use with care.
 */
#define OJL_CLEAR(p) ((p)->length=0)
#define OJL_TRUNCATE(p,s) (((s)<(p)->length)?((p)->length=(s)):0)
#define OJL_APPEND(p,c) (((p)->length == (p)->allocation)?0:\
((p)->cards[(p)->length++]=(c)))
#define OJL_POP(p) ((0==(p)->length)?0:(p)->cards[--(p)->length])
#define OJL_GET(p,i) ((p)->cards[i])
#define OJL_SET(p,i,c) ((p)->cards[i]=(c))


/* PROTOTYPES */

extern int oj_init_library(int seed);

/* deckinfo.c */
extern int ojd_ntypes(void);
extern int ojd_size(oj_deck_type_t t);
extern oj_cardlist_t *ojd_deck(oj_deck_type_t t);

/* text.c */
extern char *ojt_card(int c);
extern char *ojt_rank(int r);
extern char *ojt_suit(int s);
extern char *ojt_fullname(int c, char *buf, int size);
extern int ojt_val(char *str);
extern int ojt_vals(char *str, int *arr, int size);

/* prng.c */
extern int ojr_seed(int seed);
extern uint16_t ojr_next16(void);
extern uint32_t ojr_next32(void);
extern uint64_t ojr_next64(void);
extern double ojr_next_double(void);
extern int ojr_rand(const int limit);
extern void ojr_shuffle(int *array, int size, int count);

/* cardlist.c */
extern int ojl_new(oj_cardlist_t *sp, int *buf, int size);
extern int ojl_build_mask(oj_cardlist_t *sp, uint64_t *mp);
extern int ojl_clear(oj_cardlist_t *sp);
extern int ojl_truncate(oj_cardlist_t *sp, int size);
extern int ojl_size(oj_cardlist_t *sp);
extern int ojl_get(oj_cardlist_t *sp, int index);
extern int ojl_set(oj_cardlist_t *sp, int index, int card);
extern int ojl_pflag(oj_cardlist_t *sp, int mask);
extern int ojl_set_pflag(oj_cardlist_t *sp, int mask);
extern int ojl_clear_pflag(oj_cardlist_t *sp, int mask);
extern int ojl_eflag(oj_cardlist_t *sp, int mask);
extern int ojl_set_eflag(oj_cardlist_t *sp, int mask);
extern int ojl_clear_eflag(oj_cardlist_t *sp, int mask);
extern void *ojl_get_extra(oj_cardlist_t *sp);
extern void ojl_set_extra(oj_cardlist_t *sp, void *udp);
extern uint32_t ojl_hash(oj_cardlist_t *sp);
extern int ojl_append(oj_cardlist_t *sp, int card);
extern int ojl_extend(oj_cardlist_t *destp, oj_cardlist_t *srcp, int count);
extern int ojl_insert(oj_cardlist_t *sp, int index, int card);
extern int ojl_pop(oj_cardlist_t *sp);
extern int ojl_delete(oj_cardlist_t *sp, int index);
extern int ojl_index(oj_cardlist_t *sp, int card);
extern int ojl_remove(oj_cardlist_t *sp, int card);
extern int ojl_copy(oj_cardlist_t *destp, oj_cardlist_t *srcp);
extern int ojl_sort(oj_cardlist_t *sp);
extern int ojl_reverse(oj_cardlist_t *sp);
extern int ojl_equal(oj_cardlist_t *sp1, oj_cardlist_t *sp2);
extern int ojl_fill(oj_cardlist_t *sp, int count, oj_deck_type_t dt);
extern int ojl_shuffle(oj_cardlist_t *sp);
extern char *ojl_text(oj_cardlist_t *sp, char *buf, int size);

/* combiner.c */
extern long long ojc_binomial(int n, int k);
extern int ojc_new(oj_combiner_t *comb, oj_cardlist_t *deck,
    oj_cardlist_t *hand, int k, long long count);
extern int ojc_next(oj_combiner_t *comb);
extern int ojc_next_random(oj_combiner_t *comb);
extern long long ojc_colex_rank(oj_cardlist_t *hand, oj_combiner_t *comb);
extern int ojc_colex_hand_at(long long rank, oj_cardlist_t *hand,
    oj_combiner_t *comb);

/* poker.c */
extern int ojp_eval5(oj_cardlist_t *sp);
extern int ojp_eval7(oj_cardlist_t *sp);
extern int ojp_best5(oj_cardlist_t *sp, oj_cardlist_t *bh);
extern int ojp_hand_info(oj_poker_hand_info_t *ip, oj_cardlist_t *sp, int val);
extern char *ojp_hand_description(oj_poker_hand_info_t *pi);

/* blackjack.c */
extern int ojb_total(const oj_cardlist_t * const sp);


#ifndef __GNUC__
#define __attribute__()
#endif

#ifdef __cplusplus
} /* end of extern "C" */
#endif /* __cplusplus */


#endif /* _OJCARDLIB_H */
