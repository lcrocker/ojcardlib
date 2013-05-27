/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Native interface for CardList class.
 */

#include <stdio.h>
#include <stddef.h>
#include <jni.h>

#include "ojcardlib.h"
#include "com_onejoker_cardlib_CardList.h"

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_CardList_nStructSize
(JNIEnv *env, jclass cls) { return sizeof(oj_cardlist_t); }

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_CardList_nAppend
(JNIEnv *env, jclass cls, jobject b, jint c) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    return ojl_append((oj_cardlist_t *)ptr, c);
}

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_CardList_nClear
(JNIEnv *env, jclass cls, jobject b) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    return ojl_clear((oj_cardlist_t *)ptr);
}

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_CardList_nDelete
(JNIEnv *env, jclass cls, jobject b, jint i) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    return ojl_delete((oj_cardlist_t *)ptr, i);
}

JNIEXPORT jboolean JNICALL Java_com_onejoker_cardlib_CardList_nEquals
(JNIEnv *env, jclass cls, jobject b, jobject c) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    jbyte *cmp = (*env)->GetDirectBufferAddress(env, c);
    return ojl_equal((oj_cardlist_t *)ptr, (oj_cardlist_t *)cmp);
}

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_CardList_nExtend
(JNIEnv *env, jclass cls, jobject d, jobject s, jint c) {
    jbyte *dp = (*env)->GetDirectBufferAddress(env, d);
    jbyte *sp = (*env)->GetDirectBufferAddress(env, s);
    return ojl_extend((oj_cardlist_t *)dp, (oj_cardlist_t *)sp, c);
}

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_CardList_nFill
(JNIEnv *env, jclass cls, jobject b, jint c, jint t) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    return ojl_fill((oj_cardlist_t *)ptr, c, t);
}

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_CardList_nGet
(JNIEnv *env, jclass cls, jobject b, jint i) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    return ojl_get((oj_cardlist_t *)ptr, i);
}

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_CardList_nHash
(JNIEnv *env, jclass cls, jobject b) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    return ojl_hash((oj_cardlist_t *)ptr);
}

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_CardList_nIndex
(JNIEnv *env, jclass cls, jobject b, jint c) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    return ojl_index((oj_cardlist_t *)ptr, c);
}

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_CardList_nInsert
(JNIEnv *env, jclass cls, jobject b, jint i, jint c) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    return ojl_insert((oj_cardlist_t *)ptr, i, c);
}

JNIEXPORT void JNICALL Java_com_onejoker_cardlib_CardList_nNew
(JNIEnv *env, jclass cls, jobject b, jint s) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    ojl_new((oj_cardlist_t *)ptr, (int *)(ptr + sizeof(oj_cardlist_t)), s);
}

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_CardList_nPop
(JNIEnv *env, jclass cls, jobject b) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    return ojl_pop((oj_cardlist_t *)ptr);
}

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_CardList_nRemove
(JNIEnv *env, jclass cls, jobject b, jint c) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    return ojl_remove((oj_cardlist_t *)ptr, c);
}

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_CardList_nReverse
(JNIEnv *env, jclass cls, jobject b) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    return ojl_reverse((oj_cardlist_t *)ptr);
}

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_CardList_nSet
(JNIEnv *env, jclass cls, jobject b, jint i, jint c) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    return ojl_set((oj_cardlist_t *)ptr, i, c);
}

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_CardList_nShuffle
(JNIEnv *env, jclass cls, jobject b) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    return ojl_shuffle((oj_cardlist_t *)ptr);
}

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_CardList_nSize
(JNIEnv *env, jclass cls, jobject b) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    return ojl_size((oj_cardlist_t *)ptr);
}

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_CardList_nSort
(JNIEnv *env, jclass cls, jobject b) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    return ojl_sort((oj_cardlist_t *)ptr);
}

JNIEXPORT jint JNICALL Java_com_onejoker_cardlib_CardList_nTruncate
(JNIEnv *env, jclass cls, jobject b, jint s) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    return ojl_truncate((oj_cardlist_t *)ptr, s);
}
