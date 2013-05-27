/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Native interface for Card class.
 */

#include <stdio.h>
#include <jni.h>

#include "ojcardlib.h"
#include "com_onejoker_cardlib_Card.h"

JNIEXPORT void JNICALL Java_com_onejoker_cardlib_Card_seed
(JNIEnv *env, jclass cls, jint seed) {
	ojr_seed(seed);
}

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_Card_rand
(JNIEnv *env, jclass cls, jint limit) {
	return ojr_rand(limit);
}
