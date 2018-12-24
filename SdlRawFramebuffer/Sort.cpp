#include "Sort.h"
#include <stdlib.h>

// Merge with minimal copies
void iterativeMergeSort(SwitchPoint arr1[], int n) {
    SwitchPoint *arr2 = (SwitchPoint*)malloc((n) * sizeof(SwitchPoint));

    // a first pass swapping pairs (as this can be done in place)
    for (int i = 0; i < n-2; i+=2) {
        if (arr1[i+1].pos < arr1[i].pos) {
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
                if (A[l].pos < A[r].pos) { // compare the two bits to be merged
                    B[t++] = A[l++];
                } else {
                    B[t++] = A[r++];
                }
            } // exhausted at least one of the merge sides

            while (l < right) { // run down left if anything left
                B[t++] = A[l++];
            }

            while (r < end) { // run down right side if anything left
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

    // finally, clean up
    free(arr2);
}
