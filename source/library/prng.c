/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Pseudo-random number generator based on the public domain JKISS by
 * David Jones of the UCL Bioinformatics Group.
 * <http://www.cs.ucl.ac.uk/staff/d.jones/GoodPracticeRNG.pdf>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#endif

#include "ojcardlib.h"

// Seed variables
static uint64_t x, y;
static uint32_t z1, c1, z2, c2;
static int _seeded = 0;

// Ring buffer for random bits. We calculate a bufferfull of random bits
// at a time, and fetch them from the buffer, refilling when needed.
#define RB_SIZE 2048
static uint16_t *rptr, ring[RB_SIZE / 2];

// Seed the PRNG. If we are passed 0, generate a good seed from system
// entropy. Otherwise, give a reproducible sequence.
int ojr_seed(int seed) {
    uint32_t s[8];
    time_t t;
#ifdef _WIN32
    HCRYPTPROV hCryptProv;
#else
    int fn;
#endif

    // Make sure we reload on first call
    rptr = ring;

    // Start with some reasonable defaults
    x = 123456789123ull;
    y = 987654321987ull;
    z1 = 43219876;
    c1 = 6543217;
    z2 = 21987643;
    c2 = 1732654;
    _seeded = 0;

    // If we were passed a nonzero seed, mix those bits in with the
    // defaults to get a repeatable sequence.
    if (0 != seed) {
        x ^= (0x5A5A5A5A & seed);
        y ^= (0xA5A5A5A5 & seed);
        _seeded = 1;
        return 0;
    }
    // Fetch seed from system entropy.
#ifdef _WIN32
    do {
        if (! CryptAcquireContext(&hCryptProv, "LDC",
            0, PROV_RSA_FULL, 0)) {
            if (! CryptAcquireContext(&hCryptProv, "LDC",
                0, PROV_RSA_FULL, CRYPT_NEWKEYSET)) {
                break;
            }
        }
        if (CryptGenRandom(hCryptProv, 32, (BYTE *)s)) _seeded = 1;
        CryptReleaseContext(hCryptProv, 0);
    } while (0);
#else
    fn = open("/dev/urandom", O_RDONLY);
    if (-1 != fn) {
        if (32 == read(fn, s, 32)) _seeded = 1;
        close(fn);
    }
#endif
    if (_seeded) {
        x = *(uint64_t *)s;
        y = *(uint64_t *)(s + 2);
        if (0ull == y) y = 987654321987ull;

        z1 = s[4];
        c1 = s[5] | (1 << 28);
        z2 = s[6];
        c2 = s[7] | (1 << 29);
        return 0;
    }
    // Fall back to using time()
    time(&t);
    x ^= (0xA5A5A5A5 & t);
    y ^= (0x5A5A5A5A & t);
    _seeded = 1;
    return 0;
}

// Need more random bits.
static void reload(void) {
    int i;
    uint64_t t;

    assert(_seeded);
    for (i = 0; i < (RB_SIZE / 8); ++i) {
        x = 1490024343005336237ull * x + 123456789;

        y ^= y << 21;
        y ^= y >> 17;
        y ^= y << 30;

        t = 4294584393ull * z1 + c1;
        c1 = t >> 32;
        z1 = t;

        t = 4246477509ull * z2 + c2;
        c2 = t >> 32;
        z2 = t;

        ((uint64_t *)ring)[i] = x + y + z1 + ((uint64_t)z2 << 32);
    }
    rptr = ring + (RB_SIZE / 2);
}

// Return next 16, 32, 64 random bits from buffer.
uint16_t ojr_next16(void) {
    assert(_seeded);
    if (rptr == ring) reload();
    return *--rptr;
}

uint32_t ojr_next32(void) {
    assert(_seeded);
    if (rptr < (ring + 2)) reload();
    rptr -= 2;
    return *((uint32_t *)rptr);
}

uint64_t ojr_next64(void) {
    assert(_seeded);
    if (rptr < (ring + 4)) reload();
    rptr -= 4;
    return *((uint64_t *)rptr);
}

// For those of you into floating point, return one in [0,1).
// Assumes ieee-64 floating point format.
static union {
    double d;
    uint64_t i;
} ieee;

double ojr_next_double(void) {
    uint64_t r = ojr_next64();
    ieee.i = (r >> 12) | 0x3FF0000000000000;
    return ieee.d - 1.0;
}

// Return a well-balanced random integer from 0 to limit-1. Limited to 16 bits!
int ojr_rand(int limit) {
    int v, m = limit - 1;
    assert(_seeded);
    assert(limit > 0);
    assert(limit < 65536);

    m |= m >> 1;
    m |= m >> 2;
    m |= m >> 4;
    m |= m >> 8;

    do {
        if (rptr == ring) reload();
        v = m & *--rptr;
    } while (v >= limit);
    return v;
}

