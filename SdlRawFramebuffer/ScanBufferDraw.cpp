#include "ScanBufferDraw.h"
#include <stdlib.h>

#include <iostream>
using namespace std;

ScanBuffer * InitScanBuffer(int width, int height)
{
    auto buf = (ScanBuffer*)calloc(1, sizeof(ScanBuffer));
    if (buf == NULL) return NULL;

    auto sizeEstimate = width * height * 2;

    // Idea: Have a single list and sort by overall position rather than x (would need a background reset at each scan start?)
    //       Could also do a 'region' like difference-from-last-scanline?

    buf->list = (SwitchPoint*)calloc(sizeEstimate, sizeof(SwitchPoint));
    if (buf->list == NULL) {
        free(buf); return NULL;
    }

    buf->itemCount = 0;
    buf->count = 0;
    buf->length = sizeEstimate;
    buf->height = height;
    buf->width = width;

    return buf;
}

void FreeScanBuffer(ScanBuffer * buf)
{
    if (buf == NULL) return;
    if (buf->list != NULL) free(buf->list);
    free(buf);
}

void GrowBuffer(ScanBuffer * buf) {
    // TODO: extend the buffer size!
    cout << "\r\nShould have extended the buffer. Will fail!";
}

// INTERNAL: Write scan switch points into buffer for a single line.
//           Used to draw any other polygons
void SetLine(
    ScanBuffer *buf,
    int x0, int y0,
    int x1, int y1,
    int z,
    int r, int g, int b)
{
    int upper, lower;
    int left;
    float grad; // probably good enough on raspi, but might do with fixed-point
    int h = buf->height;
    int w = buf->width;
    uint32_t flags;

    if (y0 == y1) return; // no scanlines would be affected
    if (y0 < y1) { // going down
        flags = 0x00; // 'off' line
        upper = (y0 < 0) ? 0 : y0;
        lower = (y1 > h) ? h : y1;
        left = x0;
        grad = (float)(x1 - x0) / (float)(y1 - y0);
    } else { // going up
        flags = 0x01; // 'on' line
        upper = (y0 < 0) ? 0 : y0;
        lower = (y1 > h) ? h : y1;
        left = x0;
        grad = (float)(x1 - x0) / (float)(y1 - y0);
    }

    uint32_t color = ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);

    int needed = lower - upper;
    int remains = buf->length - buf->count;
    if (needed > remains) GrowBuffer(buf);
    int yoffs = upper - y0; //???

    for (auto i = 0; i <= needed; i++)
    {
        // add a point.
        auto ox = (grad * (i + yoffs)) + x0;
        uint32_t addr = ((i + upper)*w) + ox;

        SwitchPoint sp;
        sp.id = buf->itemCount;
        sp.pos = addr;
        sp.material = color;
        sp.meta = flags;// 0x01: set = 'on' point, unset = 'off' point

        buf->list[buf->count] = sp;
        buf->count++;
    }

}

// Fill a triagle with a solid colour
// Triangle must be clockwise winding (if dy is -ve, line is 'on', otherwise line is 'off')
// counter-clockwise contours are not handled, and will lead to weird effects, like holes (feel free to abuse)
void FillTrangle(
    ScanBuffer *buf, 
    int x0, int y0,
    int x1, int y1,
    int x2, int y2,
    int z,
    int r, int g, int b)
{
    if (buf == NULL) return;
    if (z < 0) return; // behind camera
    buf->itemCount++;

    SetLine(buf,   x0, y0, x1, y1,    z, r, g, b);
    SetLine(buf,   x1, y1, x2, y2,    z, r, g, b);
    SetLine(buf,   x2, y2, x0, y0,    z, r, g, b);
}

// Reset all drawing operations in the buffer, ready for next frame
// Do this *after* rendering to pixel buffer
void ClearScanBuffer(ScanBuffer * buf)
{
    if (buf == NULL) return;
    buf->itemCount = 0; // reset object ids
    buf->count = 0; // set occupancy to zero. All the old scanline values remain, but we ignore them
}

// Sorting the scan buffer
// TODO: resolve depth and point conflicts
void InPlaceSort(SwitchPoint *list, int left, int right)
{
    if (left >= right) { return; } // done
    int lo = left;
    int hi = right;

    int mid = lo + ((hi - lo) / 2);

    // Partition the list into two lists and sort them recursively
    InPlaceSort(list, lo, mid);
    InPlaceSort(list, mid + 1, hi);

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

// Render a scan buffer to a pixel framebuffer
// This can be done on a different processor core from other draw commands to spread the load
// Do not draw to a buffer while it is rendering (switch buffers if you need to)
void RenderBuffer(
    ScanBuffer *buf,                           // source scan buffer
    BYTE* data, int rowBytes,                  // target frame-buffer
    int left, int top, int right, int bottom   // area of target buffer to fill
) {
    if (buf == NULL || data == NULL) return;

    // Plan: implement some kind of fused sort+normalise algorithm so we can then
    //       run through all the pixels (ignoring on/off and z-depth) rendering dumbly.
    //       maybe also have a specialised version that doesn't have the target area.

    auto list = buf->list;
    auto count = buf->count; // this might need updating after sort/split
    InPlaceSort(list, 0, count);

    int dataTop = top * rowBytes;
    int dataBot = bottom * rowBytes;
    int dataLeft = left * 4;
    int dataRight = right * 4;
    
    // Temp -- just draw between ON and OFF
    // we scan through the sorted list to find a point where the position switches from before current to after current.
    // we then count how many pixel until next switch
    // then we loop, writing those pixels (if we're inside the mask region)
    // in regions where there is no fill defined, we don't update the existing pixel buffer (transparent)

    auto cur_pos = dataTop + dataLeft;
    auto end_pos = dataBot + dataRight;
    /*
    while (cur_pos < end_pos) {
        auto idx_prev = LastIndexBefore(list, cur_pos, count);
        auto idx_next = FirstIndexAfter(list, cur_pos, count);

        if (idx_prev < 0.........

        SwitchPoint pl = list[.........];
        auto stop = cur_pos + (idx_next - idx_......);

        for (int i = cur_pos; i < stop; i++)
        {
            ......
        }
    }*/

    
    for (int y = dataTop; y < dataBot; y+= rowBytes)
    {
        for (int x = dataLeft; x < dataRight; x+=4)
        {
            // for initial testing, just draw a color
            auto pixoff = y + x;
            data[pixoff + 0] = 200; // b
            data[pixoff + 1] = 70;  // g
            data[pixoff + 2] = 128; // r
        }
    }
}

// NOTES:

// Backgrounds: To set a general background color, the first position (possibly at pos= -1) should be an 'ON' at the furthest depth.
//              There should be no matching 'OFF'.
//              In areas where there is no fill present, no change to the existing image is made.

// Holes: A CCW winding polygon will have 'OFF's before 'ON's, being inside-out. If a single 'ON' is set before this shape
//        (Same as a background) then we will fill only where the polygon is *not* present -- this makes vignette effects simple

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

Sorted [list | heap] of switch points

*/