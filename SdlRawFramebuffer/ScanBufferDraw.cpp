#include "ScanBufferDraw.h"
#include "BinHeap.h"
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

    buf->heap = Initialize(10);
    if (buf->heap == NULL) {
        free(buf->list);
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
    if (buf->heap != NULL) Destroy((PriorityQueue)buf->heap);
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
    uint8_t flags;
    int adj; // 'off' pixels are pushed 1 to the right

    if (y0 == y1) {
        return; // no scanlines would be affected
    }
    if (y0 < y1) { // going down
        flags = 0x00; // 'off' line
        upper = (y1 < 0) ? 0 : y1;
        lower = (y0 > h) ? h : y0;
        left = x0;
        adj = 1;
        grad = (float)(x1 - x0) / (float)(y1 - y0);
    } else { // going up
        flags = 0x01; // 'on' line
        upper = (y0 < 0) ? 0 : y0;
        lower = (y1 > h) ? h : y1;
        left = x0;
        adj = 0;
        grad = (float)(x1 - x0) / (float)(y0 - y1);
    }

    uint32_t color = ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);

    int needed = upper - lower;
    int remains = buf->length - buf->count;
    if (needed > remains) GrowBuffer(buf);

    for (auto i = 0; i <= needed; i++)
    {
        // add a point.
        auto ox = (grad * i) + x0;
        uint32_t addr = ((i + lower)*w) + ox + adj;

        SwitchPoint sp;
        sp.id = buf->itemCount;
        sp.pos = addr;
        sp.material = color;
        sp.meta = flags;

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
    ScanBuffer *buf,             // source scan buffer
    BYTE* data, int rowBytes,    // target frame-buffer
    int bufSize                  // size of target buffer
) {
    if (buf == NULL || data == NULL) return;

    // Plan: implement some kind of fused sort+normalise algorithm so we can then
    //       run through all the pixels (ignoring on/off and z-depth) rendering dumbly.
    //       maybe also have a specialised version that doesn't have the target area.

    InPlaceSort(buf->list, 0, buf->count);
    auto list = buf->list;
    auto count = buf->count; // this might need updating after sort/split
    
    int end = bufSize / 4; // end of data in 32bit words

    bool on = false;
    int p = 0; // current pixel
    uint32_t color;
    for (int i = 0; i < count; i++)
    {
        SwitchPoint sw = list[i];

        if (sw.pos > p) {
            if (on) {
                for (; p < sw.pos; p++)
                {
                    if (p >= end) return;
                    ((uint32_t*)data)[p] = color;
                }
            } else p = sw.pos;
        }
        on = sw.meta == 0x01;
        color = sw.material;

#if 0
        // DEBUG: show switch point in black
        int pixoff = ((sw.pos - 1) * 4);
        if (pixoff > 0) { data[pixoff + 0] = data[pixoff + 1] = data[pixoff + 2] = 0; }
        // END
#endif
    }

    if (on) { // fill to end of data
        for (; p < end; p++)
        {
            ((uint32_t*)data)[p] = color;
        }
    }

    /*
        Insert(ElementType{2, 200}, heap);
        Insert(ElementType{20, 2000}, heap);
        Insert(ElementType{3, 300}, heap);
        Insert(ElementType{1, 100}, heap);
        Insert(ElementType{15, 1500}, heap);

        while (!IsEmpty(heap)) {
            ElementType tmp = DeleteMin(heap);
            cout << "Found depth = " << tmp.depth << ", idx = " << tmp.index << ";\r\n";
        }
        */
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