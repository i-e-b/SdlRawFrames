#pragma once
#include "ScanBufferDraw.h"

// 'iterative' quick sort
// keeps it's own stack rather than using the call stack
void quickSort(SwitchPoint *arr, int l, int h);

// in-place merge sort
// merging is more operations, but memory use is very low
void inPlaceMergeSort(SwitchPoint *list, int left, int right);

// Iterative mergesort function to sort arr[0...n-1]
// Very fast, but uses lots of extra memory
void iterativeMergeSort(SwitchPoint arr[], int n);