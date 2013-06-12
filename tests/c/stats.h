/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Statistics routines for various test programs.
 */

struct counter {
    int n;
    long total, clipped;
    double mn, mx, rw;
    long counts[1];
};

struct counter *create_counter(int);
void free_counter(struct counter *);
void set_counter_range(struct counter *, double, double);
double counter_report(struct counter *);

#define INC(c,b) do { ++(c)->total; \
if ((b) < 0 || (b) >= (c)->n) ++(c)->clipped; \
else ++(c)->counts[b]; } while (0)

#define INCV(c,v) do { ++(c)->total; \
if ((v) <= (c)->mn || (v) >= (c)->mx) ++(c)->clipped; \
else ++(c)->counts[(int)(((v) - (c)->mn) * (c)->rw)]; } while (0)
