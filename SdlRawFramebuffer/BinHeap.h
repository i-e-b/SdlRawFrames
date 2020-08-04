#pragma once
#ifndef BinHeap_H
#define BinHeap_H

// This type is used for the ScanBuffer drawing SwitchPoint list
typedef struct ElementType {
    int depth;      // the 'priority' of our element. Should be greater than zero
    int identifier; // a unique identifier for the element (Note: internally, this is used as a 2ndary priority to add sort stability)
    int lookup;     // any extra information needed. Does not need to be unique.
} ElementType;


struct HeapStruct;
typedef struct HeapStruct *PriorityQueue;

// Allocate and setup a heap structure with a given size
PriorityQueue HeapInit(int MaxElements);
// Deallocate a heap
void HeapDestroy(PriorityQueue H);
// Remove all entries without deallocating ( O(1) time )
void HeapMakeEmpty(PriorityQueue H);
// Add an element ( O(log n) )
void HeapInsert(ElementType X, PriorityQueue H);
// Remove the minimum element, returning its value ( O(log n) )
ElementType HeapDeleteMin(PriorityQueue H);
// Returning the value of the minimum element ( O(1) )
ElementType HeapPeekMin(PriorityQueue H);
// Returning the value of the minimum element, testing for its existence first ( O(1) )
bool HeapTryFindMin(PriorityQueue H, ElementType *found);
// Return the value of the second-minimum element, if present ( O(1) )
bool HeapTryFindNext(PriorityQueue H, ElementType *found);
// Returns true if heap has no elements
int HeapIsEmpty(PriorityQueue H);
// Returns true if heap has no spare capacity
int HeapIsFull(PriorityQueue H);

#endif