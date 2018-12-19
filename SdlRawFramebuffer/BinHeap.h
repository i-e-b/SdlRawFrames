#pragma once
#ifndef _BinHeap_H
#define _BinHeap_H

// This type is used for the ScanBuffer drawing SwitchPoint list
typedef struct ElementType {
    int depth; // the 'priority' of our element
    int index; // a look-up into another data store
} ElementType;


struct HeapStruct;
typedef struct HeapStruct *PriorityQueue;

PriorityQueue Initialize(int MaxElements);
void Destroy(PriorityQueue H);
void MakeEmpty(PriorityQueue H);
void Insert(ElementType X, PriorityQueue H); // maybe split depth & index?
ElementType DeleteMin(PriorityQueue H);
ElementType FindMin(PriorityQueue H);
int IsEmpty(PriorityQueue H);
int IsFull(PriorityQueue H);

#endif