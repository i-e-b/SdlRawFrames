#include "Sort.h"
#include <stdlib.h>
#include <iostream>

inline bool cmp(SwitchPoint* a, int idx1, int idx2) {
    // first sort by position
    auto p1 = a[idx1].xpos;
    auto p2 = a[idx2].xpos;
    if (p1 < p2) return true;
    if (p1 > p2) return false;

    // they are equal. Now sort by object ID
    auto id1 = a[idx1].id;
    auto id2 = a[idx2].id;
    if (id1 < id2) return true;
    if (id1 > id2) return false;

    // still equal. Put on before off
    auto s1 = a[idx1].state;
    auto s2 = a[idx2].state;
    return (s1 > s2); // note this is reversed!
}

// Merge with minimal copies
void iterativeMergeSort(SwitchPoint* source, SwitchPoint* tmp, int n) {
    if (n < 2) return;

    auto arr1 = source;
    auto arr2 = tmp;

    // a first pass swapping pairs (as this can be done in place)
    for (int i = 1; i < n - 3; i += 2) { // a run offset by one
        if (cmp(arr1, i + 1, i)) {
            auto tmp = arr1[i]; arr1[i] = arr1[i + 1]; arr1[i + 1] = tmp;
        }
    }
    for (int i = 0; i < n - 2; i += 2) { // 2^n aligned run (critical to the merge algorithm)
        if (cmp(arr1, i + 1, i)) {
            auto tmp = arr1[i]; arr1[i] = arr1[i + 1]; arr1[i + 1] = tmp;
        }
    }

    auto A = arr2; // we will be flipping the array pointers around
    auto B = arr1;

    for (int stride = 2; stride < n; stride *= 2) { // doubling merge width
        
        // swap A and B pointers after each merge set
        { auto tmp = A; A = B; B = tmp; }

        int t = 0; // incrementing point in target array
        for (int left = 0; left < n; left += 2 * stride) {
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

    // if we just wrote to the aux buffer, copy everything back.
    if (B == arr2) {
        for (int i = 0; i < n; i++) {
            arr1[i] = arr2[i];
        }
    }
}
