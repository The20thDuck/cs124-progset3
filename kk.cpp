#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include "assert.h"
#include "alternativesols.hh"
#include <sys/time.h>
#include <cstdint>

#include <random>
#include <chrono>

#define N 100
#define NUMCASES 100
#define MAXA 1000000000000 // 10^12

class MaxHeap {
public:
    long harr[N];
    int capacity;
    int heap_size;
    MaxHeap(int cap) {
        heap_size = 0;
        capacity = cap;
        assert(cap <= N);
    }
    int parent(int i) {
        return (i-1)/2;
    }
    int left(int i) {
        return 2*i+1;
    }
    int right(int i) {
        return 2*i+2;
    }
    void maxHeapify(int i) {
        int l = left(i);
        int r = right(i);
        int largest;
        if (l < heap_size && harr[l] > harr[i]) {
            largest = l;
        }
        else {
            largest = i;
        }
        if (r < heap_size && harr[r] > harr[largest]) {
            largest = r;
        }
        if (largest != i) {
            std::swap(harr[i], harr[largest]);
        }
    }
    void buildHeap() {
        for (int i = heap_size/2; i >= 0; i--) {
            maxHeapify(i);
        }
    }
    long extractMax() {
        assert(heap_size > 0);
        long max = harr[0];
        harr[0] = harr[heap_size-1];
        heap_size--;
        maxHeapify(0);
        return max;
    }
    void insert(long v) {
        heap_size++;
        assert(heap_size <= N);
        int pos = heap_size-1;
        harr[pos] = v;
        while (pos >= 0 && harr[parent(pos)] < harr[pos]) {
            std::swap(harr[parent(pos)], harr[pos]);
            pos = parent(pos);
        }
    }
    void printHeap() {
        for (int i = 0; i < heap_size; i++) {
            printf("%ld ", harr[i]);
        }
        printf("\n");
    }
};


long karmarkarKarp(MaxHeap h) {
    while (h.heap_size > 1) {
        long a = h.extractMax();
        long b = h.extractMax();
        h.insert(abs(a-b));
    }
    return h.extractMax();
}

long karmarkarKarp(long arr[], int n) {
    MaxHeap h(n);
    for (int i = 0; i < n; i++) {
        h.insert(arr[i]);
    }
    return karmarkarKarp(h);
}

long testCases[NUMCASES][N];

void test() {

	// rng seeding
    struct timeval t;
    gettimeofday(&t, NULL);
    srand((unsigned) t.tv_usec);


    // Generate random instances
    std::random_device rd;     //Get a random seed from the OS entropy device, or whatever
    std::mt19937_64 eng(rd()); //Use the 64-bit Mersenne Twister 19937 generator
                                //and seed it with entropy.
    std::uniform_int_distribution<long> dist(1, MAXA);
    
    for (int i = 0; i < NUMCASES; i++) {
        for (int j = 0; j < N; j++) {
            testCases[i][j] = dist(eng);
        }
        // printf("%ld\n", testCases[i][0]);
    }

    
    for (int method = 3; method < 7; method++) {
        long total_residue = 0;
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        for (int i = 0; i < NUMCASES; i++) {
            long residue;
            switch (method)
            {
            case 0:
                residue = karmarkarKarp(testCases[i], N);
                break;

            case 1:
                residue = repeatedRandom(testCases[i], N, 1);
                break;
            
            case 2:
                residue = repeatedRandom(testCases[i], N, 0);
                break;
            
            case 3:
                residue = hillClimbing(testCases[i], N, 1);
                break;
            
            case 4:
                residue = hillClimbing(testCases[i], N, 0);
                break;

            case 5:
                residue = simulatedAnnealing(testCases[i], N, 1);
                break;
            
            case 6:
                residue = simulatedAnnealing(testCases[i], N, 0);
                break;

            default:
                break;
            }
            total_residue += residue;
        }
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        int t = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        printf("Method %d, ave residue: %ld, Time (ms): %d\n", method, total_residue/NUMCASES, t);
    }
}


int main(int argc, char* argv[]) {
    if (argc == 1) {
        test();
    }
    if (argc == 2) {
        char* line = NULL;
        size_t len;
        FILE* input = fopen(argv[1], "r");
        
        MaxHeap h(N);
        for (int i = 0; i < N; i++) {
            int r = getline(&line, &len, input);
            assert (r != -1);
            long d = atol(line);
            h.insert(d);
        }
        long dif = karmarkarKarp(h);
        printf("%ld\n", dif);
    }
}