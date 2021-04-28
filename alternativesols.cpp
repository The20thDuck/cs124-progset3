#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "kk.hh"
#include <assert.h>

#define MAX_ITER 25000
#define N 100

// Sequence generator
void seqGen(int n, int s[]) 
{

	// Assign randomly to sets
	for (int i = 0; i < n; i++) 
    {
		int set = rand() & 1;

		s[i] = (set == 1) ? 1 : -1;
	}

	return;
}



// Generates a random prepartitioning )
void partGen(int n, int s[]) {
    // Assign randomly to partition
    for (int i = 0; i < n; i++) 
    {
    	s[i] = rand() % n;
    }

    return;
}

// Generates a random neighbor of a sequence and returns mutated sequence
void seqNext(int s[], int neighbor[], int n) {

	for (int i = 0; i < n; i++) 
    {
		neighbor[i] = s[i];
	}

	int i = rand() % n;
	neighbor[i] = -neighbor[i];

	
	if ((double) rand() / RAND_MAX > 0.5) {
		int j = i;
		while (j == i) 
        {
			j = rand() % n;
		}
		neighbor[j] = -neighbor[j];
	}
	return;
}

// Produces a random neighbor of a sequence
void partNext(int s[], int neighbor[], int n) {
	// Produce neighbor
	for (int i = 0; i < n; i++) 
    {
		neighbor[i] = s[i];
	}

	// Produce neighbor
	int i = rand() % n;
	int j = neighbor[i];

	// Ensure that different number gets assigned
	while (j == neighbor[i]) 
    {
		j = rand() % n;
	}

	neighbor[i] = j;

	return;
}


// Calculates residue for a sequence solution
long seqRes(long* nums, int* s, int n) {
    long residue = 0;
    for (int i = 0; i < n; i++) {
        residue += s[i] * nums[i];
    }
    return abs(residue);
}


int std[3][N]; 
int S = 0;

long partNums[N];
int prePart[3][N];
int P = 0;

// Calculates residue for a prepartition solution
long partRes(long* nums, int* s, int n) {
    // Calculate new numbers based on prepartition
    for (int i = 0; i < n; i++) 
    {
        partNums[i] = 0;
    }
    for (int j = 0; j < n; j++) 
    {
        partNums[s[j]] += nums[j];
    }

    // Get residue
    long residue = karmarkarKarp(partNums, n);

    return residue;
}

// Cooling function for simulated annealing
double cooling(int ITER) 
{
    return pow(10, 10) * pow(0.8, ITER / 300);
}

// Uses repeated random to find a solution to number partition
long repeatedRandom(long* nums, int n, int seq) 
{

    assert(n <= N);


    if (seq) 
    {
        seqGen(n, std[S]);
        long optResidue = seqRes(nums, std[S], n);
        for (int i = 0; i < MAX_ITER; i++) 
        {
            // Obtain random solution
            seqGen(n, std[1-S]);
            long residue = seqRes(nums, std[1-S], n);

            // If better, replace
            if (residue < optResidue) 
            {
                optResidue = residue;
                S = 1-S; // Swap from 0 to 1, or vice-versa
            }

        }

        return optResidue;
    } 
    else 
    {
        partGen(n, prePart[P]);
        long optResidue = partRes(nums, prePart[P], n);
        for (int i = 0; i < MAX_ITER; i++) 
        {
            // Obtain random solution
            partGen(n, prePart[1-P]);
            long residue = partRes(nums, prePart[1-P], n);

            // If better, replace
            if (residue < optResidue) 
            {
                optResidue = residue;
                S = 1-S;
            }
        }

        return optResidue;
    }
}

// Uses hill climbing to find a solution
long hillClimbing(long* nums, int n, int seq) 
{
    // Keep track of s; can't use passed in pointer (mutable)
    assert(n <= N);

    if (seq) 
    {
        seqGen(n, std[S]);
        long bestResidue = seqRes(nums, std[S], n);
        for (int i = 0; i < MAX_ITER; i++) 
        {
            // Get neighbor
            seqNext(std[S], std[1-S], n);
            long residue = seqRes(nums, std[1-S], n);

            // If better, replace
            if (residue < bestResidue) 
            {
                bestResidue = residue;
                S = 1-S;
            } 


        }

        return bestResidue;
    } else {
        partGen(n, prePart[P]);
        long bestResidue = partRes(nums, prePart[P], n);
        for (int i = 0; i < MAX_ITER; i++) 
        {
            // Obtain random solution
            partNext(prePart[P], prePart[1-P], n);
            long residue = partRes(nums, prePart[1-P], n);

            // If better, replace
            if (residue < bestResidue) 
            {
                bestResidue = residue;
                P = 1-P;
            } 
        }

        return bestResidue;
    }
}



// Uses simulated annealing to find a solution
long simulatedAnnealing(long* nums, int n, int isSequence) 
{

    if (isSequence) {
        seqGen(n, std[S]);
        for (int i = 0; i < n; i++) 
        {
            std[2][i] = std[S][i];
        } 

        long sResidue = seqRes(nums, std[S], n);
        long bestResidue = sResidue;
        
        for (int i = 0; i < MAX_ITER; i++) 
        {

            seqNext(std[S], std[1-S], n);
            long residue = seqRes(nums, std[1-S], n);


            if (residue < sResidue || (double) rand() / RAND_MAX < exp((long) (sResidue - residue) / cooling(i))) 
            {
                sResidue = residue;
                S = 1-S;
            }


            if (sResidue < bestResidue) 
            {
                bestResidue = sResidue;
                for (int j = 0; j < n; j++) 
                {
                    std[2][j] = std[S][j];
                }
            }


        }
        return bestResidue;
    } 
    
    else 
    {
        partGen(n, prePart[P]);
        for (int i = 0; i < n; i++) 
        {
            prePart[2][i] = prePart[P][i];
        } 
        long sResidue = partRes(nums, prePart[P], n);
        long bestResidue = sResidue;
        for (int i = 0; i < MAX_ITER; i++) 
        {

            partNext(prePart[P], prePart[1-P], n);
            long residue = partRes(nums, prePart[1-P], n);


            if (residue < sResidue || (double) rand() / RAND_MAX < exp((long) (sResidue - residue) / cooling(i))) 
            {
                sResidue = residue;
                P = 1-P;
            } 

            if (sResidue < bestResidue) 
            {
                bestResidue = sResidue;
                for (int j = 0; j < n; j++) 
                {
                    prePart[2][j] = prePart[P][j];
                }
            }
        }
        
        return bestResidue;
    }
}