#include "Sort.h"
#include <stdlib.h>

// Utility function to find minimum of two integers 
#define min(x,y)   (x < y) ? (x) : (y)

/* Function to merge the two haves arr[l..m] and arr[m+1..r] of array arr[] */
void merge(SwitchPoint arr[], SwitchPoint *aux, int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    // TODO: change this to not require the double-copy
    // (don't have L and R separate?)

    /* place temp arrays in the aux buffer */
    SwitchPoint *L = aux;
    SwitchPoint *R = &(aux[n1]);

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++) L[i] = arr[l + i];
    for (j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i].pos <= R[j].pos) { // comparison is here
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there are any */
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there are any */
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

/* Iterative mergesort function to sort arr[0...n-1] */
void iterativeMergeSort(SwitchPoint arr[], int n)
{
    int curr_size;  // For current size of subarrays to be merged 
                    // curr_size varies from 1 to n/2 
    int left_start; // For picking starting index of left subarray 
                    // to be merged 

    SwitchPoint *aux = (SwitchPoint*)malloc((n + 1) * sizeof(SwitchPoint));

    // Merge subarrays in bottom up manner.  First merge subarrays of 
    // size 1 to create sorted subarrays of size 2, then merge subarrays 
    // of size 2 to create sorted subarrays of size 4, and so on. 
    for (curr_size = 1; curr_size <= n - 1; curr_size = 2 * curr_size) {

        // Pick starting point of different subarrays of current size 
        for (left_start = 0; left_start < n - 1; left_start += 2 * curr_size) {
            // Find ending point of left subarray. mid+1 is starting  
            // point of right 
            int mid = left_start + curr_size - 1;

            int right_end = min(left_start + 2 * curr_size - 1, n - 1);

            // Merge Subarrays arr[left_start...mid] & arr[mid+1...right_end] 
            merge(arr, aux, left_start, mid, right_end);
        }
    }
    free(aux);
}
