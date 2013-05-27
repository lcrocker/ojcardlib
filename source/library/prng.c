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

/* TODO: at some point, make this into an independent library
 * with more complete features, like keeping state in a structure
 * to allow for multiple streams, keeping the system seeds for
 * reuse, multiple algorithms, etc.
 */

/* Seed variables */
static uint32_t x, y, z, c;
static int _seeded = 0;

/* Ring buffer for random bits. We calculate a bufferfull of random bits
 * at a time, and fetch them from the buffer, refilling when needed.
 */
#define RB_SIZE 2048
static uint16_t *rptr, ring[RB_SIZE / 2];

/* Seed the PRNG. If we are passed 0, generate a good seed from system
 * entropy. Otherwise, give a reproducible sequence.
 */
int ojr_seed(int seed) {
    uint32_t s[4];
    time_t t;
#ifdef _WIN32
    HCRYPTPROV hCryptProv;
#else
    int fn;
#endif

    /* Make sure we reload on first call */
    rptr = ring;

    /* Start with some reasonable defaults */
    x = 123456789;
    y = 987654321;
    z = 43219876;
    c = 6543217;
    _seeded = 0;

    /* If we were passed a nonzero seed, mix those bits in with the
     * defaults to get a repeatable sequence.
     */
    if (0 != seed) {
        x ^= (0x5A5A5A5A & seed);
        z ^= (0xA5A5A5A5 & seed);
        _seeded = 1;
        return 0;
    }
    /* Fetch seed from system entropy.
     */
#ifdef _WIN32
    do {
        if (! CryptAcquireContext(&hCryptProv, "LDC",
            0, PROV_RSA_FULL, 0)) {
            if (! CryptAcquireContext(&hCryptProv, "LDC",
                0, PROV_RSA_FULL, CRYPT_NEWKEYSET)) {
                break;
            }
        }
        if (CryptGenRandom(hCryptProv, 16, (BYTE *)s)) _seeded = 1;
        CryptReleaseContext(hCryptProv, 0);
    } while (0);
#else
    fn = open("/dev/urandom", O_RDONLY);
    if (-1 != fn) {
        if (16 == read(fn, s, 16)) _seeded = 1;
        close(fn);
    }
#endif
    if (_seeded) {
        x = s[0];
        if (0 != s[1]) y = s[1];
        z = s[2];
        c = s[3] % 698769068 + 1;
        return 0;
    }
    /* Fall back to using time()
     */
    time(&t);
    x ^= (0xA5A5A5A5 & t);
    z ^= (0x5A5A5A5A & t);
    _seeded = 1;
    return 0;
}

/* Need more random bits.
 */
__attribute__((hot))
static void reload(void) {
    int i;
    uint64_t t;

    assert(_seeded);
    for (i = 0; i < (RB_SIZE / 4); ++i) {
        x = 314527869 * x + 1234567;
        y ^= y << 5;
        y ^= y >> 7;
        y ^= y << 22;
        t = 4294584393ull * z + c;
        c = t >> 32;
        z = t;
        ((uint32_t *)ring)[i] = x + y + z;
    }
    rptr = ring + (RB_SIZE / 2);
}

/* Return next 16 random bits from buffer.
 */
__attribute__((hot))
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

/* For those of you into floating point, return one in [0,1).
 * Assumes ieee-64 floating point format.
 */
static union {
    double d;
    uint64_t i;
} ieee;

double ojr_next_double(void) {
    uint64_t r = ojr_next64();
    ieee.i = (r >> 12) | 0x3FF0000000000000;
    return ieee.d - 1.0;
}

/* Return a well-balanced random integer from 0 to limit-1.
 * Limited to 16 bits!
 */
__attribute__((hot))
int ojr_rand(const int limit) {
    int v, m = limit - 1;
    assert(_seeded);
    assert(limit > 0);
    assert(limit < 65536);

    m |= m >> 1;
    m |= m >> 2;
    m |= m >> 4;
    m |= m >> 8;

    do {
        v = ojr_next16() & m;
    } while (v >= limit);
    return v;
}

#define SWAP(a,b) do{t=array[a];array[a]=array[b];array[b]=t;}while(0)

/* Move to the top of the array a randomly-chosen combination of <count>
 * elements, where each combination and permutation is equally likely.
 * If <count> == <size>, this becomes a standard Fisher-Yates shuffle.
 */
void ojr_shuffle(int *array, int size, int count) {
    int i, r, t;

    if (size < 2) return;
    if (count == size) --count;

    for (i = 0; i < count; ++i) {
        r = ojr_rand(size - i);
        if (i != i + r) SWAP(i, i + r);
    }
}
