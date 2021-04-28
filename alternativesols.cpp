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

		s[i] = (set != 1) ? -1 : 1;
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
void seqNext(int s[], int next[], int n) {
    int r = rand() % n;

	for (int i = 0; i < n; i++) 
    {
		next[i] = s[i];
	}

	
	next[r] = -next[r];

	int j = r;
	if ((double) rand() / RAND_MAX <= 0.5) 
    {
		
		while (j == r) 
        {
			j = rand() % n;
		}
		next[j] = -next[j];
	}
	return;
}

// Produces a random neighbor of a sequence
void partNext(int s[], int next[], int n) {
	int r = rand() % n;
    
    // Produce neighbor
	for (int i = 0; i < n; i++) 
    {
		next[i] = s[i];
	}

	// Produce neighbor
	
	int j = next[r];

	// Ensure that different number gets assigned
	while (j == next[r]) 
    {
		j = rand() % n;
	}

	next[r] = j;

	return;
}


// Calculates residue for a sequence solution
long seqRes(long* nums, int* s, int n) 
{
    long res = 0;
    for (int i = 0; i < n; i++) 
    {
        res += s[i] * nums[i];
    }
    return abs(res);
}


int std[3][N]; // Store current best standard representation, as well as random standard rep. [2] holds overall best for simulated annealing
int S = 0; // Stores index of current best standard

long partNums[N]; // Store standard representation of prepartition
int prePart[3][N]; // Store current best prepartion representation, as well as random standard rep. [2] holds overall best for simulated annealing
int P = 0; // Stores index of current best standard

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

// solution from repeated random
long repeatedRandom(long* nums, int n, int seq) 
{

    assert(n <= N);


    if (seq) 
    {
        seqGen(n, std[S]);
        long optResidue = seqRes(nums, std[S], n);
        for (int i = 0; i < MAX_ITER; i++) 
        {
            // Get the rand sol
            seqGen(n, std[1-S]);
            long residue = seqRes(nums, std[1-S], n);

            // Change with better rand sol
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
            // Get the rand sol
            partGen(n, prePart[1-P]);
            long residue = partRes(nums, prePart[1-P], n);

            // Change with better rand sol
            if (residue < optResidue) 
            {
                optResidue = residue;
                S = 1-S;
            }
        }

        return optResidue;
    }
}

// solution found by hill climbing
long hillClimbing(long* nums, int n, int seq) 
{

    assert(n <= N);

    if (seq) 
    {
        seqGen(n, std[S]);
        long bestResidue = seqRes(nums, std[S], n);
        for (int i = 0; i < MAX_ITER; i++) 
        {
            
            seqNext(std[S], std[1-S], n);
            long residue = seqRes(nums, std[1-S], n);

            
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
            // Get the rand sol
            partNext(prePart[P], prePart[1-P], n);
            long residue = partRes(nums, prePart[1-P], n);

            // Change with better rand sol
            if (residue < bestResidue) 
            {
                bestResidue = residue;
                P = 1-P;
            } 
        }

        return bestResidue;
    }
}



// solution from simulated annealing 
long simulatedAnnealing(long* nums, int n, int isSequence) 
{

    if (isSequence) {
        seqGen(n, std[S]);
        for (int i = 0; i < n; i++) 
        {
            std[2][i] = std[S][i];
        } 

        long sRes = seqRes(nums, std[S], n);
        long bRes = sRes;
        
        for (int i = 0; i < MAX_ITER; i++) 
        {

            seqNext(std[S], std[1-S], n);
            long residue = seqRes(nums, std[1-S], n);


            if (((double) rand() / RAND_MAX) < exp((long) (sRes - residue) / cooling(i)) || residue < sRes) 
            {
                sRes = residue;
                S = 1-S;
            }


            if (sRes < bRes) 
            {
                bRes = sRes;
                for (int j = 0; j < n; j++) 
                {
                    std[2][j] = std[S][j];
                }
            }


        }
        return bRes;
    } 
    
    else 
    {
        partGen(n, prePart[P]);
        for (int i = 0; i < n; i++) 
        {
            prePart[2][i] = prePart[P][i];
        } 
        long sRes = partRes(nums, prePart[P], n);
        long bRes = sRes;
        for (int i = 0; i < MAX_ITER; i++) 
        {

            partNext(prePart[P], prePart[1-P], n);
            long residue = partRes(nums, prePart[1-P], n);


            if (((double) rand() / RAND_MAX) < exp((long) (sRes - residue) / cooling(i)) || residue < sRes) 
            {
                sRes = residue;
                P = 1-P;
            } 

            if (sRes < bRes) 
            {
                bRes = sRes;
                for (int j = 0; j < n; j++) 
                {
                    prePart[2][j] = prePart[P][j];
                }
            }
        }
        
        return bRes;
    }
}