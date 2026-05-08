/* @(#)rnd.c	2.1.8.2 
 *
 * 
 * RANDOM.C -- Implements Park & Miller's "Minimum Standard" RNG
 * 
 * (Reference:  CACM, Oct 1988, pp 1192-1201)
 * 
 * NextRand:  Computes next random integer
 * UnifInt:   Yields an long uniformly distributed between given bounds 
 * UnifReal: ields a real uniformly distributed between given bounds   
 * Exponential: Yields a real exponentially distributed with given mean
 * 
 */

#include "config.h"
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "dss.h"
#include "rnd.h"

char *env_config PROTO((char *tag, char *dflt));
void NthElement(long, long *);

void
dss_random(long *tgt, long lower, long upper, long stream)
{
	*tgt = UnifInt((long)lower, (long)upper, (long)stream);
	Seed[stream].usage += 1;

	return;
}

void
row_start(int t)	\
{
	int i;
	for (i=0; i <= MAX_STREAM; i++) 
		Seed[i].usage = 0 ; 
	
	return;
}

void
row_stop(int t)	\
	{ 
	int i;
	
	/* need to allow for handling the master and detail together */
	if (t == ORDER_LINE)
		t = ORDER;
	if (t == PART_PSUPP)
		t = PART;
	
	for (i=0; i <= MAX_STREAM; i++)
		if ((Seed[i].table == t) || (Seed[i].table == tdefs[t].child))
			{ 
			if (set_seeds && (Seed[i].usage > Seed[i].boundary))
				{
				fprintf(stderr, "\nSEED CHANGE: seed[%d].usage = %d\n", 
					i, Seed[i].usage); 
				Seed[i].boundary = Seed[i].usage;
				} 
			else 
				{
				NthElement((Seed[i].boundary - Seed[i].usage), &Seed[i].value);
				}
			} 
		return;
	}

void
dump_seeds(int tbl)
{
	int i;

	for (i=0; i <= MAX_STREAM; i++)
		if (Seed[i].table == tbl)
			printf("%d:\t%ld\n", i, Seed[i].value);
	return;
}

/******************************************************************

   NextRand:  Computes next random integer

*******************************************************************/

/*
 * Park-Miller Lehmer LCG: X_{n+1} = (16807 * X_n) mod (2^31 - 1).
 *
 * Schrage's method (the classic implementation) avoids 32-bit overflow
 * via two divisions and a conditional add. On a 64-bit machine that is
 * unnecessary: the product fits in a uint64_t and we can reduce mod
 * (2^31 - 1) with shift+add, no division needed. The output is the
 * same integer sequence, so any data produced from it is byte-identical
 * to the original implementation.
 */
long
NextRand(long nSeed)
{
    /*
     * prod < 2^46 (since nSeed < 2^31 and 16807 < 2^15). Mod a Mersenne
     * prime M = 2^31 - 1 reduces to (low31 + high) since 2^31 ≡ 1 (mod M).
     * After one fold the result is < 2*M, so a single conditional
     * subtraction finishes the reduction.
     */
    uint64_t prod   = (uint64_t)(unsigned long)nSeed * 16807ULL;
    uint64_t result = (prod & 0x7FFFFFFFULL) + (prod >> 31);
    if (result >= 0x7FFFFFFFULL)
        result -= 0x7FFFFFFFULL;
    return (long)result;
}

/******************************************************************

   UnifInt:  Yields an long uniformly distributed between given bounds

*******************************************************************/

/*
 * long UnifInt( long nLow, long nHigh, long nStream )
 */
long
UnifInt(long nLow, long nHigh, long nStream)

/*
 * Returns an integer uniformly distributed between nLow and nHigh, 
 * including * the endpoints.  nStream is the random number stream.   
 * Stream 0 is used if nStream is not in the range 0..MAX_STREAM.
 */

{
    double          dRange;
    long            nTemp;

    if (nStream < 0 || nStream > MAX_STREAM)
        nStream = 0;

    if (nLow > nHigh)
    {
        nTemp = nLow;
        nLow = nHigh;
        nHigh = nTemp;
    }

    dRange = DOUBLE_CAST (nHigh - nLow + 1);
    Seed[nStream].value = NextRand(Seed[nStream].value);
    nTemp = (long) (((double) Seed[nStream].value / dM) * (dRange));
    return (nLow + nTemp);
}



/******************************************************************

   UnifReal:  Yields a real uniformly distributed between given bounds

*******************************************************************/

/*
 * double UnifReal( double dLow, double dHigh, long nStream )
 */
double
UnifReal(double dLow, double dHigh, long nStream)

/*
 * Returns a double uniformly distributed between dLow and dHigh,   
 * excluding the endpoints.  nStream is the random number stream.   
 * Stream 0 is used if nStream is not in the range 0..MAX_STREAM.
 */

{
    double          dTemp;

    if (nStream < 0 || nStream > MAX_STREAM)
        nStream = 0;
    if (dLow == dHigh)
        return (dLow);
    if (dLow > dHigh)
    {
        dTemp = dLow;
        dLow = dHigh;
        dHigh = dTemp;
    }
    Seed[nStream].value = NextRand(Seed[nStream].value);
    dTemp = ((double) Seed[nStream].value / dM) * (dHigh - dLow);
    return (dLow + dTemp);
}



/******************************************************************%

   Exponential:  Yields a real exponentially distributed with given mean

*******************************************************************/

/*
 * double Exponential( double dMean, long nStream )
 */
double
Exponential(double dMean, long nStream)

/*
 * Returns a double uniformly distributed with mean dMean.  
 * 0.0 is returned iff dMean <= 0.0. nStream is the random number 
 * stream. Stream 0 is used if nStream is not in the range 
 * 0..MAX_STREAM.
 */

{
    double          dTemp;

    if (nStream < 0 || nStream > MAX_STREAM)
        nStream = 0;
    if (dMean <= 0.0)
        return (0.0);

    Seed[nStream].value = NextRand(Seed[nStream].value);
    dTemp = (double) Seed[nStream].value / dM;        /* unif between 0..1 */
    return (-dMean * log(1.0 - dTemp));
}
