#include "ScanBufferDraw.h"
#include "Sort.h"
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

    buf->p_heap = Initialize(100);
    if (buf->p_heap == NULL) {
        free(buf->list);
        return NULL;
    }

    buf->r_heap = Initialize(100);
    if (buf->r_heap == NULL) {
        Destroy((PriorityQueue)buf->p_heap);
        free(buf->list);
        return NULL;
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
    if (buf->p_heap != NULL) Destroy((PriorityQueue)buf->p_heap);
    if (buf->r_heap != NULL) Destroy((PriorityQueue)buf->r_heap);
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
    if (y0 == y1) {
        return; // ignore: no scanlines would be affected
    }

    uint32_t color = ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
    int h = buf->height;
    int w = buf->width;
    uint8_t flags;
    
    if (y0 < y1) { // going down
        flags = 0x00; // 'off' line
    } else { // going up
        flags = 0x01; // 'on' line
        // swap coords so we can always calculate down (always 1 entry per y coord)
        int tmp;
        tmp = x0; x0 = x1; x1 = tmp;
        tmp = y0; y0 = y1; y1 = tmp;
    }

    int yoff = (y0 < 0) ? -y0 : 0;
    int top = (y0 < 0) ? 0 : y0;
    int bottom = (y1 > h) ? h : y1;
    float grad = (float)(x0 - x1) / (float)(y0 - y1);

    int scanlines = bottom - top;
    int remains = buf->length - buf->count;
    if (scanlines > remains) GrowBuffer(buf);

    for (auto i = 0; i < scanlines; i++) // skip the last pixel to stop double-counting
    {
        // add a point.
        int ox = (int)(grad * (i + yoff)) + x0;
        if (ox < 0) ox = 0; // saturate edges for clipping (without this shapes wrap horizontally)
        if (ox > w) ox = w;
        uint32_t addr = ((i + top) * w) + ox;

        SwitchPoint sp;
        sp.id = buf->itemCount;
        sp.pos = addr;
        sp.depth = z;
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

// Set a single 'on' point at the given level. Nice and simple
void SetBackground(
    ScanBuffer *buf,
    int z, // depth of the background. Anything behind this will be invisible
    int r, int g, int b) {

    SwitchPoint sp;
    sp.id = buf->itemCount++;
    sp.pos = 0; // top-left of image
    sp.depth = z;
    sp.material = ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
    sp.meta = 0x01; // 'on'

    buf->list[buf->count] = sp;
    buf->count++;
}

// Reset all drawing operations in the buffer, ready for next frame
// Do this *after* rendering to pixel buffer
void ClearScanBuffer(ScanBuffer * buf)
{
    if (buf == NULL) return;
    buf->itemCount = 0; // reset object ids
    buf->count = 0; // set occupancy to zero. All the old scanline values remain, but we ignore them
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

    // TODO: sorting takes a lot of the time up. Anything we can do to improve it will help frame rates
    // If nothing else, this could go on a 3rd core, moving us to a 3 frame latency
    //quickSort(buf->list, 0, buf->count - 1);
    //inPlaceMergeSort(buf->list, 0, buf->count - 1);
    iterativeMergeSort(buf->list, buf->count);
    
    auto list = buf->list;
    auto count = buf->count;
    auto p_heap = (PriorityQueue)buf->p_heap;   // presentation heap
    auto r_heap = (PriorityQueue)buf->r_heap;   // removal heap
    
    MakeEmpty(p_heap);
    MakeEmpty(r_heap);

    uint32_t end = bufSize / 4; // end of data in 32bit words

    bool on = false;
    uint32_t p = 0; // current pixel
    uint32_t color;
    for (int i = 0; i < count; i++)
    {
        SwitchPoint sw = list[i];

        if (sw.pos > p) { // render up to this switch point
            if (on) {
                for (; p < sw.pos; p++)
                {
                    if (p >= end) return;
                    ((uint32_t*)data)[p] = color;
                }
            } else p = sw.pos;
        }

        auto heapElem = ElementType{ /*depth:*/ sw.depth, /*unique id:*/ sw.id, /*lookup index:*/ i };
        if (sw.meta & 0x01) { // 'on' point, add to presentation heap
            Insert(heapElem, p_heap);
        } else { // 'off' point, add to removal heap
            Insert(heapElem, r_heap);
        }

        // while top of p_heap and r_heap match, remove both.
        auto nextRemove = ElementType{ 0,-1,0 };
        auto top = ElementType{ 0,-1,0 };
        while (TryFindMin(p_heap, &top) && TryFindMin(r_heap, &nextRemove)
            && top.identifier == nextRemove.identifier) {
            DeleteMin(r_heap); // how do I stop double-stop and double-start?
            DeleteMin(p_heap);
        }

        // set color for next run based on top of p_heap
        on = ! IsEmpty(p_heap);
        if (on) {
            color = list[top.lookup].material;
        }

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
}

// NOTES:

// Backgrounds: To set a general background color, the first position (possibly at pos= -1) should be an 'ON' at the furthest depth.
//              There should be no matching 'OFF'.
//              In areas where there is no fill present, no change to the existing image is made.

// Holes: A CCW winding polygon will have 'OFF's before 'ON's, being inside-out. If a single 'ON' is set before this shape
//        (Same as a background) then we will fill only where the polygon is *not* present -- this makes vignette effects simple
