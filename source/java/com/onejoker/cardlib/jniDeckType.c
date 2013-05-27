/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 */

#include <stdio.h>
#include <jni.h>

#include "ojcardlib.h"
#include "com_onejoker_cardlib_DeckType.h"

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_DeckType_nTypes
(JNIEnv *env, jclass cls) {
    return ojd_ntypes();
}

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_DeckType_nCards
(JNIEnv *env, jclass cls, jint t) {
    return ojd_size(t);
}
