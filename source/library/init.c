/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * This file contains just the library initialization functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "ojcardlib.h"

int _oj_johnnymoss = 0;

#ifdef _WIN32

#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    int r;

    if (DLL_PROCESS_ATTACH == fdwReason) {
        r = oj_init_library(0);
        if (r) exit(EXIT_FAILURE);
    }
    return TRUE;
}

#else /* This is the Linux Way */

void _init(void) {
    int r;

    r = oj_init_library(0);
    if (r) exit(EXIT_FAILURE);
    return;
}

void _fini(void) {
    return;
}

#endif

int oj_init_library(int seed) {
    int r;

    r = ojr_seed(seed);
    if (r) return r;

    _oj_johnnymoss = 0x10ACE0FF;
    return 0;
}
