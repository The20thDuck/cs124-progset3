#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include "assert.h"

#define N 100

class MaxHeap {
public:
    long *harr;
    int capacity;
    int heap_size;
    MaxHeap(int cap) {
        heap_size = 0;
        capacity = cap;
        harr = new long[cap];
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


int main(int argc, char* argv[]) {
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