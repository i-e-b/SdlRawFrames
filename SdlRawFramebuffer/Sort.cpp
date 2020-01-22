#include "Sort.h"
#include <stdlib.h>
#include <iostream>

// maximal/exhaustive sort
inline bool cmp_other(SwitchPoint* a, int idx1, int idx2) {
    // first sort by position
    auto p1 = a[idx1].xpos;
    auto p2 = a[idx2].xpos;
    if (p1 < p2) return true;
    if (p1 > p2) return false;

    // then sort by object
    p1 = a[idx1].id;
    p2 = a[idx2].id;
    if (p1 < p2) return true;
    if (p1 > p2) return false;

    // still equal. Put on before off
    auto s1 = a[idx1].state;
    auto s2 = a[idx2].state;
    return (s1 > s2); // note this is reversed!
}

// minimal sort
inline bool cmp(SwitchPoint* a, int idx1, int idx2) {
    // sort by position, with `on` to the left of `off`
    auto p1 = (a[idx1].xpos << 1) + a[idx1].state;
    auto p2 = (a[idx2].xpos << 1) + a[idx2].state;
    return (p1 < p2);
}

inline int cmpval(SwitchPoint p) {
	return (p.xpos << 1) + p.state;
}

void quicksort_rec(SwitchPoint* A, int len) {
  if (len < 2) return;
 
  int pivotPoint = len / 2;
  int pivot = cmpval(A[pivotPoint]);
 
  int i, j;
  for (i = 0, j = len - 1; ; i++, j--) {
    //while (cmp(A, i, pivotPoint)) i++;
    //while (cmp(A, pivotPoint, j)) j--;
    while (cmpval(A[i]) < pivot) i++;
    while (cmpval(A[j]) > pivot) j--;
 
    if (i >= j) break;
 
    auto temp = A[i];
    A[i]     = A[j];
    A[j]     = temp;
  }
 
  quicksort_rec(A, i);
  quicksort_rec(A + i, len - i);
}

SwitchPoint* QuickSort(SwitchPoint* source, SwitchPoint* tmp, int n) {
    quicksort_rec(source, n);
    return source;
}

// most significant bit first in-place radix sort.
void radsort_rec(SwitchPoint* A, int from, int to, int bit)
{
	if (!bit || to < from + 1) return;
 
    int leftIdx = from;
    int rightIdx = to - 1;

	for (;;) {
		// find left most with bit, and right most without bit
		while (leftIdx < rightIdx && !(cmpval(A[leftIdx]) & bit)) leftIdx++;
		while (leftIdx < rightIdx &&  (cmpval(A[rightIdx]) & bit)) rightIdx--;

		if (leftIdx >= rightIdx) break; // not found

        // swap
		auto temp = A[leftIdx];
		A[leftIdx] = A[rightIdx];
		A[rightIdx] = temp;
	}
 
	if (!(cmpval(A[leftIdx]) & bit) && leftIdx < to) leftIdx++;
	bit >>= 1;
 
	radsort_rec(A, from, leftIdx, bit);
	radsort_rec(A, leftIdx, to, bit);
}

SwitchPoint* RadixSort(SwitchPoint* source, SwitchPoint* tmp, int n) {
	radsort_rec(source, 0, n, 1 << 12); // the bit is very sensitive to structure of 'SwitchPoint'
    return source;
}

// Merge with minimal copies
SwitchPoint* IterativeMergeSort(SwitchPoint* source, SwitchPoint* tmp, int n) {
    if (n < 2) return source;

    auto arr1 = source;
    auto arr2 = tmp;

    auto A = arr2; // we will be flipping the array pointers around
    auto B = arr1;

    for (int stride = 1; stride < n; stride <<= 1) { // doubling merge width
        
        // swap A and B pointers after each merge set
        { auto tmp = A; A = B; B = tmp; }

        int t = 0; // incrementing point in target array
        for (int left = 0; left < n; left += stride << 1) {
            int right = left + stride;
            int end = right + stride;
            if (end > n) end = n; // some merge windows will run off the end of the data array
            if (right > n) right = n; // some merge windows will run off the end of the data array
            int l = left, r = right; // the point we are scanning though the two sets to be merged.

            // copy the lowest candidate across from A to B
            while (l < right && r < end) {
                if (cmp(A, l, r)) { // compare the two bits to be merged
                    B[t++] = A[l++];
                } else {
                    B[t++] = A[r++];
                }
            } // exhausted at least one of the merge sides

            while (l < right) { // run down left if anything remains
                B[t++] = A[l++];
            }

            while (r < end) { // run down right side if anything remains
                B[t++] = A[r++];
            }
        }
    }

    return B; // return the actual result, whatever that is.
}
