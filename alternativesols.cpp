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

// Generates a random neighbor of a sequence and returns mutated sequence
void seqNext(int s[], int neighbor[], int n) {
	// Produce neighbor
	for (int i = 0; i < n; i++) 
    {
		neighbor[i] = s[i];
	}

	// Change sequence
	int i = rand() % n;
	neighbor[i] = -neighbor[i];

	// Change second one with probability 0.5
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

// Generates a random prepartitioning (using [0, n))
void partGen(int n, int s[]) {
    // Assign randomly to partition
    for (int i = 0; i < n; i++) 
    {
    	s[i] = rand() % n;
    }

    return;
}

// Produces a random neighbor of a sequence
// Returns mutated sequence
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

// Converts a sequence solution into a partition solution
int* sequenceToPartition(int* s, int n) 
{
	int* p;
	for (int i = 0; i < n; i++) {
		p[i] = (s[i] == 1) ? 1 : 0;
	}
	return p;
}

// Prints set passed in, whether solution or prepartition
void printSolution(int* s, int n) {
	for (int i = 0; i < n; i++) {
		printf("%4i ", s[i]);
	}
	printf("\n");
}

// Calculates residue for a sequence solution
long seqRes(long* nums, int* sign, int n) {
    long residue = 0;
    for (int i = 0; i < n; i++) {
        residue += sign[i] * nums[i];
    }
    return abs(residue);
}


int standard[3][N]; // Store current best standard representation, as well as random standard rep. [2] holds overall best for simulated annealing
int bestS = 0; // Stores index of current best standard

long newNums[N]; // Store standard representation of prepartition
int prePart[3][N]; // Store current best prepartion representation, as well as random standard rep. [2] holds overall best for simulated annealing
int bestP = 0; // Stores index of current best standard

// Calculates residue for a prepartition solution
long partitionResidue(long* nums, int* s, int n) {
    // Calculate new numbers based on prepartition
    for (int i = 0; i < n; i++) 
    {
        newNums[i] = 0;
    }
    for (int j = 0; j < n; j++) 
    {
        newNums[s[j]] += nums[j];
    }

    // Get residue
    long residue = karmarkarKarp(newNums, n);

    return residue;
}

// Uses repeated random to find a solution to number partition
long repeatedRandom(long* nums, int n, int isSequence) 
{
    // Keep track of s; can't use passed in pointer (mutable)
    assert(n <= N);

    // for (int i = 0; i < n; i++) {
    //     s[i] = start[i];
    // }

    if (isSequence) 
    {
        seqGen(n, standard[bestS]);
        long bestResidue = seqRes(nums, standard[bestS], n);
        for (int i = 0; i < MAX_ITER; i++) 
        {
            // Obtain random solution
            seqGen(n, standard[1-bestS]);
            long residue = seqRes(nums, standard[1-bestS], n);

            // If better, replace
            if (residue < bestResidue) 
            {
                bestResidue = residue;
                bestS = 1-bestS; // Swap from 0 to 1, or vice-versa
            }

        }

        return bestResidue;
    } else {
        partGen(n, prePart[bestP]);
        long bestResidue = partitionResidue(nums, prePart[bestP], n);
        for (int i = 0; i < MAX_ITER; i++) 
        {
            // Obtain random solution
            partGen(n, prePart[1-bestP]);
            long residue = partitionResidue(nums, prePart[1-bestP], n);

            // If better, replace
            if (residue < bestResidue) 
            {
                bestResidue = residue;
                bestS = 1-bestS;
            }
        }

        return bestResidue;
    }
}

// Uses hill climbing to find a solution
long hillClimbing(long* nums, int n, int isSequence) 
{
    // Keep track of s; can't use passed in pointer (mutable)
    assert(n <= N);

    if (isSequence) 
    {
        seqGen(n, standard[bestS]);
        long bestResidue = seqRes(nums, standard[bestS], n);
        for (int i = 0; i < MAX_ITER; i++) 
        {
            // Get neighbor
            seqNext(standard[bestS], standard[1-bestS], n);
            long residue = seqRes(nums, standard[1-bestS], n);

            // If better, replace
            if (residue < bestResidue) 
            {
                bestResidue = residue;
                bestS = 1-bestS;
            } 


        }

        return bestResidue;
    } else {
        partGen(n, prePart[bestP]);
        long bestResidue = partitionResidue(nums, prePart[bestP], n);
        for (int i = 0; i < MAX_ITER; i++) 
        {
            // Obtain random solution
            partNext(prePart[bestP], prePart[1-bestP], n);
            long residue = partitionResidue(nums, prePart[1-bestP], n);

            // If better, replace
            if (residue < bestResidue) 
            {
                bestResidue = residue;
                bestP = 1-bestP;
            } 
        }

        return bestResidue;
    }
}

// Cooling function for simulated annealing
double cooling(int ITER) 
{
    return pow(10, 10) * pow(0.8, ITER / 300);
}

// Uses simulated annealing to find a solution
long simulatedAnnealing(long* nums, int n, int isSequence) 
{

    if (isSequence) {
        seqGen(n, standard[bestS]);
        for (int i = 0; i < n; i++) 
        {
            standard[2][i] = standard[bestS][i];
        } 
        
        // Copy current to overall best
        long sResidue = seqRes(nums, standard[bestS], n);
        long bestResidue = sResidue;
        for (int i = 0; i < MAX_ITER; i++) 
        {
            // Get neighbor
            seqNext(standard[bestS], standard[1-bestS], n);
            long residue = seqRes(nums, standard[1-bestS], n);

            // If better or within probability, replace
            if (residue < sResidue || (double) rand() / RAND_MAX < exp((long) (sResidue - residue) / cooling(i))) 
            {
                sResidue = residue;
                bestS = 1-bestS;
            }

            // Reassign best solution
            if (sResidue < bestResidue) 
            {
                bestResidue = sResidue;
                for (int j = 0; j < n; j++) 
                {
                    standard[2][j] = standard[bestS][j];
                }
            }


        }
        return bestResidue;
    } else 
    {
        partGen(n, prePart[bestP]);
        for (int i = 0; i < n; i++) 
        {
            prePart[2][i] = prePart[bestP][i];
        } // Copy current to overall best
        long sResidue = partitionResidue(nums, prePart[bestP], n);
        long bestResidue = sResidue;
        for (int i = 0; i < MAX_ITER; i++) 
        {
            // Get neighbor
            partNext(prePart[bestP], prePart[1-bestP], n);
            long residue = partitionResidue(nums, prePart[1-bestP], n);

            // If better or within probability, replace
            if (residue < sResidue || (double) rand() / RAND_MAX < exp((long) (sResidue - residue) / cooling(i))) 
            {
                sResidue = residue;
                bestP = 1-bestP;
            } 
            // Reassign best solution
            if (sResidue < bestResidue) 
            {
                bestResidue = sResidue;
                for (int j = 0; j < n; j++) 
                {
                    prePart[2][j] = prePart[bestP][j];
                }
            }
        }
        
        return bestResidue;
    }
}