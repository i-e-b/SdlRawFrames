#include "ScanBufferDraw.h"

#include "Sort.h"
#include "BinHeap.h"

#include <cstdlib>
using namespace std;

#define ON 0x01
#define OFF 0x00

#define OBJECT_MAX 65535

// NOTES:

// Backgrounds: To set a general background color, the first position (possibly at pos= -1) should be an 'ON' at the furthest depth per scanline.
//              There should be no matching 'OFF'.
//              In areas where there is no fill present, no change to the existing image is made.

// Holes: A CCW winding polygon will have 'OFF's before 'ON's, being inside-out. If a single 'ON' is set before this shape
//        (Same as a background) then we will fill only where the polygon is *not* present -- this makes vignette effects simple

ScanBuffer * InitScanBuffer(int width, int height)
{
    auto buf = (ScanBuffer*)calloc(1, sizeof(ScanBuffer));
    if (buf == nullptr) return nullptr;

    auto sizeEstimate = width * 2;
    buf->expectedScanBufferSize = sizeEstimate;

    // Idea: Have a single list and sort by overall position rather than x (would need a background reset at each scan start?)
    //       Could also do a 'region' like difference-from-last-scanline?

    buf->materials = (Material*)calloc(OBJECT_MAX + 1, sizeof(Material));
    if (buf->materials == nullptr) { FreeScanBuffer(buf); return nullptr; }

    buf->scanLines = (ScanLine*)calloc(height+1, sizeof(ScanLine)); // we use a spare line as sorting temp memory
    if (buf->scanLines == nullptr) { FreeScanBuffer(buf); return nullptr; }

    for (int i = 0; i < height + 1; i++) {
        auto scanBuf = (SwitchPoint*)calloc(sizeEstimate + 1, sizeof(SwitchPoint));
        if (scanBuf == nullptr) { FreeScanBuffer(buf); return nullptr; }
        buf->scanLines[i].points = scanBuf;
        buf->scanLines[i].count = 0;
        buf->scanLines[i].length = sizeEstimate;
    }

    buf->p_heap = HeapInit(OBJECT_MAX);
    if (buf->p_heap == nullptr) {
        FreeScanBuffer(buf);
        return nullptr;
    }

    buf->r_heap = HeapInit(OBJECT_MAX);
    if (buf->r_heap == nullptr) {
        FreeScanBuffer(buf);
        return nullptr;
    }

    buf->itemCount = 0;
    buf->height = height;
    buf->width = width;

    return buf;
}

void FreeScanBuffer(ScanBuffer * buf)
{
    if (buf == nullptr) return;
    if (buf->scanLines != nullptr) {
        for (int i = 0; i < buf->height; i++) {
            if (buf->scanLines[i].points != nullptr) free(buf->scanLines[i].points);
        }
        free(buf->scanLines);
    }
    if (buf->materials != nullptr) free(buf->materials);
    if (buf->p_heap != nullptr) HeapDestroy((PriorityQueue)buf->p_heap);
    if (buf->r_heap != nullptr) HeapDestroy((PriorityQueue)buf->r_heap);
    free(buf);
}

// Set a point with an exact position, clipped to bounds
// gradient is 0..15; 15 = vertical; 0 = near horizontal.
inline void SetSP(ScanBuffer * buf, int x, int y, uint16_t objectId, uint8_t isOn) {
    if (y < 0 || y > buf->height) return;
    
   // SwitchPoint sp;
    ScanLine* line = &(buf->scanLines[y]);

	if (line->count >= line->length) return; // buffer full. TODO: grow?

    auto idx = line->count;
    auto points = line->points;

    points[idx].xpos = (x < 0) ? 0 : x;
    points[idx].id = objectId;
    points[idx].state = isOn;

	line->dirty = true; // ensure it's marked dirty
	line->count++; // increment pointer
}

inline void SetMaterial(ScanBuffer* buf, uint16_t objectId, int depth, uint32_t color) {
    if (objectId > OBJECT_MAX) return;
    buf->materials[objectId].color = color;
    buf->materials[objectId].depth = depth;
}

// INTERNAL: Write scan switch points into buffer for a single line.
//           Used to draw any other polygons
void SetLine(
    ScanBuffer *buf,
    int x0, int y0,
    int x1, int y1,
    int z,
    uint32_t r, uint32_t g, uint32_t b)
{
    if (y0 == y1) {
        return; // ignore: no scanlines would be affected
    }

    uint32_t color = ((r & 0xffu) << 16u) + ((g & 0xffu) << 8u) + (b & 0xffu);
    int h = buf->height;
    //int w = buf->width;
    uint8_t isOn;
    
    if (y0 < y1) { // going down
        isOn = OFF;
    } else { // going up
        isOn = ON;
        // swap coords so we can always calculate down (always 1 entry per y coord)
        int tmp;
        tmp = x0; x0 = x1; x1 = tmp;
        tmp = y0; y0 = y1; y1 = tmp;
    }

    //int yoff = (y0 < 0) ? -y0 : 0;
    int top = (y0 < 0) ? 0 : y0;
    int bottom = (y1 > h) ? h : y1;
    float grad = (float)(x0 - x1) / (float)(y0 - y1);

    auto objectId = buf->itemCount;
    SetMaterial(buf, objectId, z, color);

    for (int y = top; y < bottom; y++) // skip the last pixel to stop double-counting
    {
        // add a point.
        int x = (int)(grad * static_cast<float>(y-y0) + static_cast<float>(x0));
        SetSP(buf, x, y, objectId, isOn);
    }

}

// Internal: Fill an axis aligned rectangle
void GeneralRect(ScanBuffer *buf,
    int left, int top, int right, int bottom,
    int z,
    int r, int g, int b)
{
    if (left >= right || top >= bottom) return; //empty
    SetLine(buf,
        left, bottom,
        left, top,
        z, r, g, b);
    SetLine(buf,
        right, top,
        right, bottom,
        z, r, g, b);
}

// Fill an axis aligned rectangle
void FillRect(ScanBuffer *buf,
    int left, int top, int right, int bottom,
    int z,
    int r, int g, int b)
{
    if (z < 0) return; // behind camera
    GeneralRect(buf, left, top, right, bottom, z, r, g, b);

    buf->itemCount++;
}

void FillCircle(ScanBuffer *buf,
    int x, int y, int radius,
    int z,
    int r, int g, int b) {
    FillEllipse(buf,
        x, y, radius * 2, radius * 2,
        z,
        r, g, b);
}


void GeneralEllipse(ScanBuffer *buf,
                    int xc, int yc, int width, int height,
                    int z, bool positive,
                    uint32_t r, uint32_t g, uint32_t b)
{
    uint32_t color = ((r & 0xffu) << 16u) + ((g & 0xffu) << 8u) + (b & 0xffu);

    uint8_t left = (positive) ? (ON) : (OFF);
    uint8_t right = (positive) ? (OFF) : (ON);

    int a2 = width * width;
    int b2 = height * height;
    int fa2 = 4 * a2, fb2 = 4 * b2;
    int x, y, ty, sigma;
    
    auto objectId = buf->itemCount;
    SetMaterial(buf, objectId, z, color);
    //int grad = 15; // TODO: calculate (could be based on distance from centre line)

    // Top and bottom (need to ensure we don't double the scanlines)
    for (x = 0, y = height, sigma = 2 * b2 + a2 * (1 - 2 * height); b2*x <= a2 * y; x++) {
        if (sigma >= 0) {
            sigma += fa2 * (1 - y);
            // only draw scan points when we change y
            SetSP(buf, xc - x, yc + y, objectId, left);
            SetSP(buf, xc + x, yc + y, objectId, right);

            SetSP(buf, xc - x, yc - y, objectId, left);
            SetSP(buf, xc + x, yc - y, objectId, right);
            y--;
        }
        sigma += b2 * ((4 * x) + 6);
    }
    ty = y; // prevent overwrite

    // Left and right
    SetSP(buf, xc - width, yc, objectId, left);
    SetSP(buf, xc + width, yc, objectId, right);
    for (x = width, y = 1, sigma = 2 * a2 + b2 * (1 - 2 * width); a2*y < b2 * x; y++) {
        if (y > ty) break; // started to overlap 'top-and-bottom'

        SetSP(buf, xc - x, yc + y, objectId, left);
        SetSP(buf, xc + x, yc + y, objectId, right);

        SetSP(buf, xc - x, yc - y, objectId, left);
        SetSP(buf, xc + x, yc - y, objectId, right);

        if (sigma >= 0) {
            sigma += fb2 * (1 - x);
            x--;
        }
        sigma += a2 * ((4 * y) + 6);
    }
}


void FillEllipse(ScanBuffer *buf,
    int xc, int yc, int width, int height,
    int z,
    int r, int g, int b)
{
    if (z < 0) return; // behind camera

    GeneralEllipse(buf,
        xc, yc, width, height,
        z, true,
        r, g, b);

    buf->itemCount++;
}

void EllipseHole(ScanBuffer *buf,
    int xc, int yc, int width, int height,
    int z,
    int r, int g, int b) {

    if (z < 0) return; // behind camera

    // set background
    GeneralRect(buf, 0, 0, buf->width, buf->height, z, r, g, b);

    // Same as ellipse, but with on and off flipped to make hole
    GeneralEllipse(buf,
        xc, yc, width, height,
        z, false,
        r, g, b);

    buf->itemCount++;
}

// Fill a quad given 3 points
void FillTriQuad(ScanBuffer *buf,
    int x0, int y0,
    int x1, int y1,
    int x2, int y2,
    int z,
    int r, int g, int b) {
    // Basically the same as triangle, but we also draw a mirror image across the xy1/xy2 plane
    if (buf == nullptr) return;
    if (z < 0) return; // behind camera

    if (x2 == x1 && x0 == x1 && y0 == y1 && y1 == y2) return; // empty

    // Cross product (finding only z)
    // this tells us if we are clockwise or ccw.
    int dx1 = x1 - x0; int dx2 = x2 - x0;
    int dy1 = y1 - y0; int dy2 = y2 - y0;
    int dz = dx1 * dy2 - dy1 * dx2;

    if (dz <= 0) { // ccw
        auto tmp = x1; x1 = x2; x2 = tmp;
        tmp = y1; y1 = y2; y2 = tmp;
        dx1 = dx2; dy1 = dy2;
    }
    SetLine(buf, x0, y0, x1, y1, z, r, g, b);
    SetLine(buf, x1, y1, x2 + dx1, y2 + dy1, z, r, g, b);
    SetLine(buf, x2 + dx1, y2 + dy1, x2, y2, z, r, g, b);
    SetLine(buf, x2, y2, x0, y0, z, r, g, b);

    buf->itemCount++;
}

float isqrt(float number) {
	unsigned long i;
	float x2, y;
	int j;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = *(long*)&y;
	i = 0x5f3759df - (i >> 1u);
	y = *(float*)&i;
	j = 3;
	while (j--) {	y = y * (threehalfs - (x2 * y * y)); }

	return y;
}

void DrawLine(ScanBuffer * buf, int x0, int y0, int x1, int y1, int z, int w, int r, int g, int b)
{
    if (w < 1) return; // empty

    // TODO: special case for w < 2

    // Use triquad and the gradient's normal to draw
    auto ndy = (float)(   x1 - x0  );
    auto ndx = (float)( -(y1 - y0) );

    // normalise
    float mag_w = static_cast<float>(w) * isqrt((ndy*ndy) + (ndx*ndx));
    ndx *= mag_w;
    ndy *= mag_w;

    int hdx = (int)(ndx / 2);
    int hdy = (int)(ndy / 2);

    // Centre points on line width 
    x0 -= hdx;
    y0 -= hdy;
    x1 -= (int)(ndx - static_cast<float>(hdx));
    y1 -= (int)(ndy - static_cast<float>(hdy));

    FillTriQuad(buf, x0, y0, x1, y1,
        x0 + (int)(ndx), y0 + (int)(ndy),
        z, r, g, b);
}

void OutlineEllipse(ScanBuffer * buf, int xc, int yc, int width, int height, int z, int w, int r, int g, int b)
{
    if (z < 0) return; // behind camera

    int w1 = w / 2;
    int w2 = w - w1;

    GeneralEllipse(buf,
        xc, yc, width + w2, height + w2,
        z, true, r, g, b);

    GeneralEllipse(buf,
        xc, yc, width - w1, height - w1,
        z, false, r, g, b);

    buf->itemCount++;
}

// Fill a triagle with a solid colour
// Triangle must be clockwise winding (if dy is -ve, line is 'on', otherwise line is 'off')
// counter-clockwise contours are detected and rearraged
void FillTrangle(
    ScanBuffer *buf, 
    int x0, int y0,
    int x1, int y1,
    int x2, int y2,
    int z,
    int r, int g, int b)
{
    if (buf == nullptr) return;
    if (z < 0) return; // behind camera

    if (x0 == x1 && x1 == x2) return; // empty
    if (y0 == y1 && y1 == y2) return; // empty

    // Cross product (finding only z)
    // this tells us if we are clockwise or ccw.
    int dx1 = x1 - x0; int dx2 = x2 - x0;
    int dy1 = y1 - y0; int dy2 = y2 - y0;
    int dz = dx1 * dy2 - dy1 * dx2;

    if (dz > 0) { // cw
        SetLine(buf, x0, y0, x1, y1, z, r, g, b);
        SetLine(buf, x1, y1, x2, y2, z, r, g, b);
        SetLine(buf, x2, y2, x0, y0, z, r, g, b);
    } else { // ccw - switch vertex 1 and 2 to make it clockwise.
        SetLine(buf, x0, y0, x2, y2, z, r, g, b);
        SetLine(buf, x2, y2, x1, y1, z, r, g, b);
        SetLine(buf, x1, y1, x0, y0, z, r, g, b);
    }

    buf->itemCount++;
}

// Set a single 'on' point at the given level on each scan line
void SetBackground(
    ScanBuffer *buf,
    int z, // depth of the background. Anything behind this will be invisible
    int r, int g, int b) {
    if (z < 0) return; // behind camera

    SetLine(buf,
        0, buf->height,
        0, 0,
        z, r, g, b);

    buf->itemCount++;
}

// Reset all drawing operations in the buffer, ready for next frame
// Do this *after* rendering to pixel buffer
void ClearScanBuffer(ScanBuffer * buf)
{
    if (buf == nullptr) return;
    buf->itemCount = 0; // reset object ids
    for (int i = 0; i < buf->height; i++)
    {
        buf->scanLines[i].count = 0;
        buf->scanLines[i].dirty = true;
    }
}

// blend two colors, by a proportion [0..255]
// 255 is 100% color1; 0 is 100% color2.
inline uint32_t Blend(uint32_t prop1, uint32_t color1, uint32_t color2) {
    if (prop1 >= 255) return color1;
    if (prop1 <= 0) return color2;

    uint32_t prop2 = 255u - prop1;
    uint32_t r = prop1 * ((color1 & 0x00FF0000u) >> 16u);
    uint32_t g = prop1 * ((color1 & 0x0000FF00u) >> 8u);
    uint32_t b = prop1 * (color1 & 0x000000FFu);

    r += prop2 * ((color2 & 0x00FF0000u) >> 16u);
    g += prop2 * ((color2 & 0x0000FF00u) >> 8u);
    b += prop2 * (color2 & 0x000000FFu);

    // everything needs shifting 8 bits, we've integrated it into the color merge
    return ((r & 0xff00u) << 8u) + ((g & 0xff00u)) + ((b >> 8u) & 0xffu);
}

// reduce display heap to the minimum by merging with remove heap
inline void CleanUpHeaps(PriorityQueue p_heap, PriorityQueue r_heap) {
    // clear first rank (ended objects that are on top)
    // while top of p_heap and r_heap match, remove both.
    auto nextRemove = ElementType{ 0,-1,0 };
    auto top = ElementType{ 0,-1,0 };
    while (HeapTryFindMin(p_heap, &top) && HeapTryFindMin(r_heap, &nextRemove)
        && top.identifier == nextRemove.identifier) {
        HeapDeleteMin(r_heap);
        HeapDeleteMin(p_heap);
    }

    // clear up second rank (ended objects that are behind the top)
    auto nextObj = ElementType{ 0,-1,0 };

    // clean up the heaps more
    if (HeapTryFindNext(p_heap, &nextObj)) {
        if (HeapPeekMin(r_heap).identifier == nextObj.identifier) {
            auto current = HeapDeleteMin(p_heap); // remove the current top (we'll put it back after)
            while (HeapTryFindMin(p_heap, &top) && HeapTryFindMin(r_heap, &nextRemove)
                && top.identifier == nextRemove.identifier) {
                HeapDeleteMin(r_heap);
                HeapDeleteMin(p_heap);
            }
            HeapInsert(current, p_heap);
        }
    }
}

void RenderScanLine(
    ScanBuffer *buf,             // source scan buffer
    int lineIndex,               // index of the line we're drawing
    BYTE* data                   // target frame-buffer
) {
	auto scanLine = &(buf->scanLines[lineIndex]);
	if (!scanLine->dirty) return;
	scanLine->dirty = false;

    auto tmpLine = &(buf->scanLines[buf->height]);

    int yoff = buf->width * lineIndex;
    auto materials = buf->materials;
    auto count = scanLine->count;
    //auto width = buf->width;

    // Note: sorting takes a lot of the time up. Anything we can do to improve it will help frame rates
    auto list = IterativeMergeSort(scanLine->points, tmpLine->points, count);
    //auto list = QuickSort(scanLine->points, tmpLine->points, count);
    //auto list = RadixSort(scanLine->points, tmpLine->points, count);

    
    auto p_heap = (PriorityQueue)buf->p_heap;   // presentation heap
    auto r_heap = (PriorityQueue)buf->r_heap;   // removal heap
    
    HeapMakeEmpty(p_heap);
    HeapMakeEmpty(r_heap);

    uint32_t end = buf->width; // end of data in 32bit words

    bool on = false;
    uint32_t p = 0; // current pixel
    uint32_t c=0,color = 0; // color of current object
    //uint32_t color_under = 0; // antialiasing color
    SwitchPoint current = {}; // top-most object's most recent "on" switch
    for (int i = 0; i < count; i++)
    {
        SwitchPoint sw = list[i];
        if (sw.xpos > end) break; // ran off the end

        Material m = materials[sw.id];

        if (sw.xpos > p) { // render up to this switch point
            if (on) {
                auto max = (sw.xpos > end) ? end : sw.xpos;
                uint32_t* d = (uint32_t*)(data + ((p+yoff) * sizeof(uint32_t)));
                for (; p < max; p++) {
                    // -- 'fade rate'
                    //if (current.fade < 15) current.fade++;

                    // -- smear / blur test
                    //c = Blend(128, color, c); // smearing blur. `prop1` Should lower for flatter angles
                    //*(d++) = c;

                    // -- plain:
                    *(d++) = color;

                } // draw pixels up to the point
            } else p = sw.xpos; // skip direct to the point
        }

        auto heapElem = ElementType{ /*depth:*/ m.depth, /*unique id:*/(int)sw.id, /*lookup index:*/ i };
        if (sw.state == ON) { // 'on' point, add to presentation heap
            HeapInsert(heapElem, p_heap);
        } else { // 'off' point, add to removal heap
            HeapInsert(heapElem, r_heap);
        }

        CleanUpHeaps(p_heap, r_heap);
        ElementType top = { 0,0,0 };
        on = HeapTryFindMin(p_heap, &top);

        if (on) {
            // set color for next run based on top of p_heap
            //color = materials[top.identifier].color;
            current = list[top.lookup];
            color = materials[current.id].color;

            // If there is another object underneath, we store the color for antialiasing.
            /*ElementType nextObj = { 0,0,0 };
            if (HeapTryFindNext(p_heap, &nextObj)) {
                color_under = materials[nextObj.identifier].color;
            } else {
                color_under = 0;
            }*/
        } else {
            color = 0;
        }


#if 0
        // DEBUG: show switch point in black
        int pixoff = ((yoff + sw.xpos - 1) * 4);
        if (pixoff > 0) { data[pixoff + 0] = data[pixoff + 1] = data[pixoff + 2] = 0; }
        // END
#endif
    } // out of switch points

    
    if (on) { // fill to end of data
        for (; p < end; p++) {
            ((uint32_t*)data)[p + yoff] = color;
        }
    }
    
}

// Render a scan buffer to a pixel framebuffer
// This can be done on a different processor core from other draw commands to spread the load
// Do not draw to a scan buffer while it is rendering (switch buffers if you need to)
void RenderBuffer(
    ScanBuffer *buf, // source scan buffer
    BYTE* data       // target frame-buffer (must match scanbuffer dimensions)
) {
    if (buf == nullptr || data == nullptr) return;

    for (int i = 0; i < buf->height; i++) {
        RenderScanLine(buf, i, data);
    }
}

