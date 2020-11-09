#include "BinHeap.h"
#include <cstdlib>

#define MinPQSize (10)
#define MinData (-32767)

struct HeapStruct {
    int Capacity;
    int Size;
    ElementType *Elements;
};

PriorityQueue HeapInit(int MaxElements) {
    PriorityQueue H;

    if (MaxElements < MinPQSize)
        return nullptr;
    
    H = (PriorityQueue) malloc(sizeof(struct HeapStruct));
    
    if (H == nullptr) return nullptr;
    
    /* Allocate the array plus one extra for sentinel */
    H->Elements = (ElementType*)malloc((MaxElements + 1) * sizeof(ElementType));
    
    if (H->Elements == nullptr) {
        free(H);
        return nullptr;
    }

    H->Capacity = MaxElements;
    H->Size = 0;
    H->Elements[0].depth = MinData;

    return H;
}

void HeapMakeEmpty(PriorityQueue H) {
    H->Size = 0;
}

// H->Element[0] is an empty value that makes the math simpler

// returns true if A > B
// should give stable results by including unique ID when priority is equal
inline bool Compare(ElementType A, ElementType B) {
    if (A.depth > B.depth) return true;
    if (A.depth < B.depth) return false;
    return A.identifier > B.identifier; // depths are equal, use identifier
}

void HeapInsert(ElementType X, PriorityQueue H) {
    if (HeapIsFull(H)) { return; }

    unsigned int i;

    for (i = ++H->Size; Compare(H->Elements[i >> 1u], X); i >>= 1u) {
        H->Elements[i] = H->Elements[i >> 1u];
    }

    H->Elements[i] = X;
}

ElementType HeapDeleteMin(PriorityQueue H) {
    int i, Child;
    ElementType MinElement, LastElement;

    if (HeapIsEmpty(H)) {
        return H->Elements[0]; // our empty value
    }
    MinElement = H->Elements[1];
    LastElement = H->Elements[H->Size--];

    for (i = 1; i * 2 <= H->Size; i = Child) {
        /* Find smaller child */
        Child = i * 2;
        if (Child != H->Size && Compare(H->Elements[Child], H->Elements[Child + 1])) Child++;

        /* Percolate one level */
        if (Compare(LastElement, H->Elements[Child])) H->Elements[i] = H->Elements[Child];
        else break;
    }
    H->Elements[i] = LastElement;
    return MinElement;
}

ElementType HeapPeekMin(PriorityQueue H) {
    if (!HeapIsEmpty(H)) return H->Elements[1];

    return H->Elements[0];
}

bool HeapTryFindMin(PriorityQueue H, ElementType *found) {
    if (!HeapIsEmpty(H)) {
        *found = H->Elements[1];
        return true;
    }
    return false;
}

// find the 2nd least element
bool HeapTryFindNext(PriorityQueue H, ElementType * found)
{
    if (H->Size < 2) return false;
    if (H->Size == 2) {
        *found = H->Elements[2];
        return true;
    }
    // inspect top two and pick the smallest
    if (Compare(H->Elements[2], H->Elements[3])) {
        *found = H->Elements[3];
    } else {
        *found = H->Elements[2];
    }

    return true;
}

int HeapIsEmpty(PriorityQueue H) {
    return H->Size == 0;
}

int HeapIsFull(PriorityQueue H) {
    return H->Size == H->Capacity;
}

void HeapDestroy(PriorityQueue H) {
    free(H->Elements);
    free(H);
}
