#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "heap.h"
#include "solution.h"

#define MAX_ITER 25000


// Calculates residue for a sequence solution
long seqRes(long* nums, int* sign, int n) {
    long residue = 0;
    for (int i = 0; i < n; i++) {
        residue += sign[i] * nums[i];
    }
    return residue;
}

// Calculates residue for a prepartition solution
long partitionResidue(long* nums, int* s, int n) {
    // Calculate new numbers based on prepartition
    long* newNums = malloc(n, sizeof(long));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (s[j] == i) {
                newNums[i] += nums[j];
            }
        }
    }

    // Get residue
    long residue = kk(newNums, n);
    free(newNums);
    return residue;
}

// Uses repeated random to find a solution to number partition
long repeatedRandom(long* nums, int* start, int n, int isSequence) {
    // Keep track of s; can't use passed in pointer (mutable)
    int* s = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        s[i] = start[i];
    }

    if (isSequence) {
        uint64_t bestResidue = seqRes(nums, s, n);
        for (int i = 0; i < MAX_ITER; i++) {
            // Obtain random solution
            int* randomSolution = sequenceGenerate(n);
            uint64_t residue = seqRes(nums, randomSolution, n);

            // If better, replace
            if (residue < bestResidue) {
                bestResidue = residue;
                free(s);
                s = randomSolution;
            } else {
                free(randomSolution);
            }

        }
        free(s);
        return bestResidue;
    } else {
        uint64_t bestResidue = partitionResidue(nums, s, n);
        for (int i = 0; i < MAX_ITER; i++) {
            // Obtain random solution
            int* randomSolution = partitionGenerate(n);
            uint64_t residue = partitionResidue(nums, randomSolution, n);

            // If better, replace
            if (residue < bestResidue) {
                bestResidue = residue;
                free(s);
                s = randomSolution;
            } else {
                free(randomSolution);
            }

                        // Write results
            // FILE* f = fopen("data/rrp.csv", "a");
            // if (f == NULL) {
            //     printf("Could not open rrp.csv\n");
            //     return -1;
            // }
            // fprintf(f, "%i,%llu,%llu\n", i, bestResidue, residue);
            // fclose(f);
        }
        free(s);
        return bestResidue;
    }
}

// Uses hill climbing to find a solution
uint64_t hillClimbing(uint64_t* nums, int* start, int n, int isSequence) {
    // Keep track of s; can't use passed in pointer (mutable)
    int* s = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        s[i] = start[i];
    }

    if (isSequence) {
        uint64_t bestResidue = seqRes(nums, s, n);
        for (int i = 0; i < MAX_ITER; i++) {
            // Get neighbor
            int* neighbor = sequenceNeighbor(s, n);
            uint64_t residue = seqRes(nums, neighbor, n);

            // If better, replace
            if (residue < bestResidue) {
                bestResidue = residue;
                free(s);
                s = neighbor;
            } else {
                free(neighbor);
            }

            // Write results
            // FILE* f = fopen("data/hcs.csv", "a");
            // if (f == NULL) {
            //     printf("Could not open hcs.csv\n");
            //     return -1;
            // }
            // fprintf(f, "%i,%llu,%llu\n", i, bestResidue, residue);
            // fclose(f);
        }
        free(s);
        return bestResidue;
    } else {
        uint64_t bestResidue = partitionResidue(nums, s, n);
        for (int i = 0; i < MAX_ITER; i++) {
            // Obtain random solution
            int* neighbor = partitionNeighbor(s, n);
            uint64_t residue = partitionResidue(nums, neighbor, n);

            // If better, replace
            if (residue < bestResidue) {
                bestResidue = residue;
                free(s);
                s = neighbor;
            } else {
                free(neighbor);
            }

            // Write results
            // FILE* f = fopen("data/hcp.csv", "a");
            // if (f == NULL) {
            //     printf("Could not open hcp.csv\n");
            //     return -1;
            // }
            // fprintf(f, "%i,%llu,%llu\n", i, bestResidue, residue);
            // fclose(f);
        }
        free(s);
        return bestResidue;
    }
}

// Cooling function for simulated annealing
double cooling(int iter) {
    return pow(10, 10) * pow(0.8, iter / 300);
}

// Uses simulated annealing to find a solution
uint64_t simulatedAnnealing(uint64_t* nums, int* start, int n, int isSequence) {
    // Keep track of s and bestS; can't use passed in pointer (mutable)
    int* s = malloc(sizeof(int) * n);
    int* bestS = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        s[i] = start[i];
        bestS[i] = start[i];
    }


    if (isSequence) {
        uint64_t sResidue = sequenceResidue(nums, s, n);
        uint64_t bestResidue = sequenceResidue(nums, bestS, n);
        for (int i = 0; i < MAX_ITER; i++) {
            // Get neighbor
            int* neighbor = sequenceNeighbor(s, n);
            uint64_t residue = sequenceResidue(nums, neighbor, n);

            // If better or within probability, replace
            if (residue < sResidue || (double) rand() / RAND_MAX < exp((int64_t) (sResidue - residue) / cooling(i))) {
                sResidue = residue;
                free(s);
                s = neighbor;
            } else {
                free(neighbor);
            }

            // Reassign best solution
            if (sResidue < bestResidue) {
                bestResidue = sResidue;
                for (int j = 0; j < n; j++) {
                    bestS[j] = s[j];
                }
            }

            // Write results
            // FILE* f = fopen("data/sas.csv", "a");
            // if (f == NULL) {
            //     printf("Could not open sas.csv\n");
            //     return -1;
            // }
            // fprintf(f, "%i,%llu,%llu,%llu\n", i, bestResidue, sResidue, residue);
            // fclose(f);
        }
        free(s);
        free(bestS);
        return bestResidue;
    } else {
        uint64_t sResidue = partitionResidue(nums, s, n);
        uint64_t bestResidue = partitionResidue(nums, bestS, n);
        for (int i = 0; i < MAX_ITER; i++) {
            // Get neighbor
            int* neighbor = partitionNeighbor(s, n);
            uint64_t residue = partitionResidue(nums, neighbor, n);

            // If better or within probability, replace
            if (residue < sResidue || (double) rand() / RAND_MAX < exp((int64_t) (sResidue - residue) / cooling(i))) {
                sResidue = residue;
                free(s);
                s = neighbor;
            } else {
                free(neighbor);
            }

            // Reassign best solution
            if (sResidue < bestResidue) {
                bestResidue = sResidue;
                for (int j = 0; j < n; j++) {
                    bestS[j] = s[j];
                }
            }
        }
        free(s);
        free(bestS);
        return bestResidue;
    }
}