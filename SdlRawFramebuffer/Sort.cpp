#include "Sort.h"
#include <stdlib.h>

void swap(SwitchPoint* a, SwitchPoint* b) {
    SwitchPoint t = *a;
    *a = *b;
    *b = t;
}

int partition(SwitchPoint arr[], int l, int h) {
    SwitchPoint x = arr[h];
    int i = (l - 1);

    for (int j = l; j <= h - 1; j++) {
        if (arr[j].pos <= x.pos) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[h]);
    return (i + 1);
}

/* arr --> Array to be sorted,
   l   --> Starting index,
   h   --> Ending index */
void quickSort(SwitchPoint *arr, int l, int h)
{
    // Create an auxiliary stack 
    int *stack;
    stack = (int*)malloc((h - l + 1) * sizeof(int));
    if (stack == NULL) return;

    // initialize top of stack 
    int top = -1;

    // push initial values of l and h to stack 
    stack[++top] = l;
    stack[++top] = h;

    // Keep popping from stack while is not empty 
    while (top >= 0) {
        // Pop h and l 
        h = stack[top--];
        l = stack[top--];

        // Set pivot element at its correct position 
        // in sorted array 
        int p = partition(arr, l, h);

        // If there are elements on left side of pivot, 
        // then push left side to stack 
        if (p - 1 > l) {
            stack[++top] = l;
            stack[++top] = p - 1;
        }

        // If there are elements on right side of pivot, 
        // then push right side to stack 
        if (p + 1 < h) {
            stack[++top] = p + 1;
            stack[++top] = h;
        }
    }
    free(stack);
}


void inPlaceMergeSort(SwitchPoint *list, int left, int right)
{
    if (left >= right) { return; } // done
    int lo = left;
    int hi = right;

    int mid = lo + ((hi - lo) / 2);

    // Partition the list into two lists and sort them recursively
    inPlaceMergeSort(list, lo, mid);
    inPlaceMergeSort(list, mid + 1, hi);

    // Merge the two sorted lists
    int end_lo = mid;
    int start_hi = mid + 1;
    while ((lo <= end_lo) && (start_hi <= hi)) {
        if (list[lo].pos < list[start_hi].pos) { // THIS IS THE COMPARE. We order by 'pos'
            // in order
            lo++;
        } else {// out of order
            // Merge items are out of order. Swap things around
            SwitchPoint temp = list[start_hi];
            for (int k = start_hi - 1; k >= lo; k--) {
                list[k + 1] = list[k];
            }
            list[lo] = temp;
            lo++;
            end_lo++;
            start_hi++;
        }
    }
}


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
