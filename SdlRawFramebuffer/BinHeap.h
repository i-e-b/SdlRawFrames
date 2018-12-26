#pragma once
#ifndef _BinHeap_H
#define _BinHeap_H

// This type is used for the ScanBuffer drawing SwitchPoint list
typedef struct ElementType {
    int depth;      // the 'priority' of our element. Should be greater than zero
    int identifier; // a unique identifier for the element (Note: internally, this is used as a 2ndary priority to add sort stability)
    int lookup;     // any extra information needed. Does not need to be unique.
} ElementType;


struct HeapStruct;
typedef struct HeapStruct *PriorityQueue;

PriorityQueue HeapInit(int MaxElements);
void HeapDestroy(PriorityQueue H);
void HeapMakeEmpty(PriorityQueue H);
void HeapInsert(ElementType X, PriorityQueue H);
ElementType HeapDeleteMin(PriorityQueue H);
ElementType HeapPeekMin(PriorityQueue H);
bool HeapTryFindMin(PriorityQueue H, ElementType *found);
int HeapIsEmpty(PriorityQueue H);
int HeapIsFull(PriorityQueue H);

#endif