#pragma once

#ifndef sort_h
#define sort_h
#include "ScanBufferDraw.h"

// Iterative mergesort function to sort arr[0...n-1]
// Very fast, but uses lots of extra memory. `source` and `tmp` should be the same size
// The final result could be in source OR tmp, so we return the pointer to the result
SwitchPoint* IterativeMergeSort(SwitchPoint* source, SwitchPoint* tmp, int n);

#endif
