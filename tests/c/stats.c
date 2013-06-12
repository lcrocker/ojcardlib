/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Statistics routines for various test programs.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <math.h>

#include "stats.h"

struct counter *create_counter(int n) {
    struct counter *c = calloc(1, (n - 1) * sizeof(long) + sizeof(struct counter));
    c->n = n;
    return c;
}
void free_counter(struct counter *c) { free(c); }

void set_counter_range(struct counter *c, double mn, double mx) {
    c->mn = mn;
    c->mx = mx;
    // Precalculate reciprocal of bucket width
    c->rw = c->n / (mx - mn);
}

#define PI 3.1415926535897932385
#define LNSQ2PI 0.9189385332046727418
#define ISQ2 0.70710678118654752444

static double lanczos[9] = {
    0.999999999999809932, 676.520368121885099, -1259.13921672240287,
    771.323428777653079, -176.615029162140599, 12.5073432786869048,
    -0.138571095265720117, 9.98436957801957086e-6, 1.50563273514931156e-7
};

static double lngamma(double z) {
    if (z < 0.5) return log(PI / sin(PI * z)) - lngamma(1.0 - z);

    z -= 1.0;
    double base = z + 7.5;
    double sum = 0;

    for (int j = 8; j > 0; --j) { sum += lanczos[j] / (z + (double)j); }
    sum += lanczos[0];
    return ((LNSQ2PI + log(sum)) - base) + log(base) * (z + 0.5);
}

#define PQITERATIONS 120
#define PQEPSILON 3e-7

static double pseries(double a, double x) {
    double ap = a;
    double sum = 1.0 / a;
    double del = sum;
    double lng = lngamma(a);

    for (int i = 0; i < PQITERATIONS; ++i) {
        ap += 1.0;
        del *= x / ap;
        sum += del;
        if (fabs(del) < fabs(sum) * PQEPSILON) {
            return sum * exp(-x + a * log(x) - lng);
        }
    }
    // Iterations maxed
    return sum * exp(-x + a * log(x) - lng);
}

static double qcfrac(double a, double x) {
    double g, gold = 0.0, fac = 1.0, b1 = 1.0;
    double anf, ana, an, a1, b0 = 0.0, a0 = 1.0;
    double lng = lngamma(a);

    a1 = x;
    for (int i = 1; i <= PQITERATIONS; ++i) {
        an = (double)i;
        ana = an - a;
        a0 = (a1 + a0 * ana) * fac;
        b0 = (b1 + b0 * ana) * fac;

        anf = an * fac;
        a1 = x * a0 + anf * a1;
        b1 = x * b0 + anf * b1;

        if (a1) {
            fac = 1.0 / a1;
            g = b1 * fac;
            if (fabs((g - gold) / g) < PQEPSILON) {
                return g * exp(-x + a * log(x) - lng);
            }
            gold = g;
        }
    }
    // Iterations maxed
    return g * exp(-x + a * log(x) - lng);
}

/*
static double igfp(double a, double x) {
    assert(a > 0.0 && x >= 0.0);
    if (x < a + 1.0) return pseries(a, x);
    else return 1.0 - qcfrac(a, x);
}
*/
static double igfq(double a, double x) {
    assert(a > 0.0 && x >= 0.0);
    if (x < a + 1.0) return 1.0 - pseries(a, x);
    else return qcfrac(a, x);
}

static double pvalue(struct counter *c) {
    double d, d2, ev = (double)(c->total) / c->n, chi2 = 0.0;

    for (int i = 0; i < c->n; ++i) {
        d = (double)(c->counts[i]) - ev;
        d2 = d * d;
        chi2 += d2 / ev;
    }
    return igfq(0.5 * (c->n - 1), 0.5 * chi2);
}

double counter_report(struct counter *c) {
    double p = pvalue(c);
    printf("%10ld values,%10ld clipped,%5d buckets, pvalue= %7.3f\n",
        c->total, c->clipped, c->n, p);
    return p;
}
