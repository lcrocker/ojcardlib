/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_onejoker_cardlib_Card */

#ifndef _Included_com_onejoker_cardlib_Card
#define _Included_com_onejoker_cardlib_Card
#ifdef __cplusplus
extern "C" {
#endif
#undef com_onejoker_cardlib_Card_DEUCE
#define com_onejoker_cardlib_Card_DEUCE 0L
#undef com_onejoker_cardlib_Card_TREY
#define com_onejoker_cardlib_Card_TREY 1L
#undef com_onejoker_cardlib_Card_FOUR
#define com_onejoker_cardlib_Card_FOUR 2L
#undef com_onejoker_cardlib_Card_FIVE
#define com_onejoker_cardlib_Card_FIVE 3L
#undef com_onejoker_cardlib_Card_SIX
#define com_onejoker_cardlib_Card_SIX 4L
#undef com_onejoker_cardlib_Card_SEVEN
#define com_onejoker_cardlib_Card_SEVEN 5L
#undef com_onejoker_cardlib_Card_EIGHT
#define com_onejoker_cardlib_Card_EIGHT 6L
#undef com_onejoker_cardlib_Card_NINE
#define com_onejoker_cardlib_Card_NINE 7L
#undef com_onejoker_cardlib_Card_TEN
#define com_onejoker_cardlib_Card_TEN 8L
#undef com_onejoker_cardlib_Card_JACK
#define com_onejoker_cardlib_Card_JACK 9L
#undef com_onejoker_cardlib_Card_QUEEN
#define com_onejoker_cardlib_Card_QUEEN 10L
#undef com_onejoker_cardlib_Card_KING
#define com_onejoker_cardlib_Card_KING 11L
#undef com_onejoker_cardlib_Card_ACE
#define com_onejoker_cardlib_Card_ACE 12L
#undef com_onejoker_cardlib_Card_RANK_JOKER
#define com_onejoker_cardlib_Card_RANK_JOKER 13L
#undef com_onejoker_cardlib_Card_CLUB
#define com_onejoker_cardlib_Card_CLUB 0L
#undef com_onejoker_cardlib_Card_DIAMOND
#define com_onejoker_cardlib_Card_DIAMOND 1L
#undef com_onejoker_cardlib_Card_HEART
#define com_onejoker_cardlib_Card_HEART 2L
#undef com_onejoker_cardlib_Card_SPADE
#define com_onejoker_cardlib_Card_SPADE 3L
#undef com_onejoker_cardlib_Card_JOKER
#define com_onejoker_cardlib_Card_JOKER 53L
#undef com_onejoker_cardlib_Card_REDJOKER
#define com_onejoker_cardlib_Card_REDJOKER 54L
/*
 * Class:     com_onejoker_cardlib_Card
 * Method:    seed
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_onejoker_cardlib_Card_seed
  (JNIEnv *, jclass, jint);

/*
 * Class:     com_onejoker_cardlib_Card
 * Method:    rand
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_Card_rand
  (JNIEnv *, jclass, jint);

#ifdef __cplusplus
}
#endif
#endif