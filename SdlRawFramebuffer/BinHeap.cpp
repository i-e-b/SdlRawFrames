#include "binheap.h"
#include <stdlib.h>
//#include "fatal.h"
//#include <stdio.h>

#define MinPQSize (10)
#define MinData (-32767)

struct HeapStruct {
    int Capacity;
    int Size;
    ElementType *Elements;
};

PriorityQueue Initialize(int MaxElements) {
    PriorityQueue H;

    if (MaxElements < MinPQSize)
        return NULL;
    
    H = (PriorityQueue) malloc(sizeof(struct HeapStruct));
    
    if (H == NULL) return NULL;
    
    /* Allocate the array plus one extra for sentinel */
    H->Elements = (ElementType*)malloc((MaxElements + 1) * sizeof(ElementType));
    
    if (H->Elements == NULL) {
        free(H);
        return NULL;
    }

    H->Capacity = MaxElements;
    H->Size = 0;
    H->Elements[0].depth = MinData;

    return H;
}

void MakeEmpty(PriorityQueue H) {
    H->Size = 0;
}

// H->Element[0] is an empty value that makes the math simpler

// returns true if A > B
// should give stable results by including unique ID when priority is equal
bool Compare(ElementType A, ElementType B) {
    if (A.depth > B.depth) return true;
    if (A.depth < B.depth) return false;
    return A.identifier > B.identifier; // depths are equal, use identifier
}

void Insert(ElementType X, PriorityQueue H) {
    if (IsFull(H)) { return; }

    int i;

    for (i = ++H->Size; Compare(H->Elements[i >> 1], X); i >>= 1)
    {
        H->Elements[i] = H->Elements[i >> 1];
    }

    H->Elements[i] = X;
}

ElementType DeleteMin(PriorityQueue H) {
    int i, Child;
    ElementType MinElement, LastElement;

    if (IsEmpty(H)) {
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

ElementType FindMin(PriorityQueue H) {
    if (!IsEmpty(H)) return H->Elements[1];

    return H->Elements[0];
}

bool TryFindMin(PriorityQueue H, ElementType *found) {
    if (!IsEmpty(H)) {
        *found = H->Elements[1];
        return true;
    }
    return false;
}

int IsEmpty(PriorityQueue H) {
    return H->Size == 0;
}

int IsFull(PriorityQueue H) {
    return H->Size == H->Capacity;
}

void Destroy(PriorityQueue H) {
    free(H->Elements);
    free(H);
}