#pragma once

// Functions to use a scan buffer
// for rendering filled shapes 

// Brain-dead way to start.
// maybe optimise later

/*

With low Z being closest.

Part overlap right
xxx8xx3xx8xx3xxx
   8  3  3  x
      8

Part overlap left
xxx3xx8xx3xx8xxx
   3  3  8  x
      8

Complete obscure
xxx3xx8xx8xx3xxx
   3  3  3  x
      8  8

Front and centre
xxx8xx3xx3xx8xxx
   8  3  8  x
      8

Conflict
   1  1  1  1
xxxAxxBxxAxxBxxx


Shared edge
   
x[8,3]xx3xx8xxx
   3    8  x
   8

Sorted tree, or heap, of span points

*/



typedef struct SwitchPoint {
    int idx; // itemCount when this point was drawn
} SwitchPoint;

typedef struct ScanBuffer {
    int itemCount;
    int width;
    int height;


} ScanBuffer;

// Fill a triagle with a solid colour
// Triangle must be counter-clockwise winding
void FillTrangle(
    ScanBuffer *buf,
    int x0, int y0, 
    int x1, int y1,
    int x2, int y2,
    int z,
    int r, int g, int b);


