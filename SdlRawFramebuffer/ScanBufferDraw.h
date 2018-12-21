#pragma once
#include <stdint.h>

#ifndef BYTE
#define BYTE unsigned char
#endif

// Functions to use a scan buffer
// for rendering filled shapes 


// entry for each 'pixel' in a scan buffer
// 'drawing' involves writing a list of these, sorting by x-position, then filling the scanline
typedef struct SwitchPoint {
    uint16_t id;                // itemCount when this point was drawn, the object ID (limit of 65k objects per frame)
    int16_t  depth;             // z-position in final image
    uint32_t pos;               // position of switch-point, as (y*width)+x;
    uint32_t material;          // RGB (24 bit of color). Could also be used to look up a texture or other style later.
    uint8_t  meta;              // metadata/flags. TODO: better alignment, merge into material?
                                // 0x01: set = 'on' point, unset = 'off' point
} SwitchPoint;

// buffer of switch points.
typedef struct ScanBuffer {
    uint16_t itemCount;      // used to give each switch-point a unique ID. This is critical for the depth-sorting process
    int height;
    int width;

    int count;              // number of items in the array
    int length;             // memory length of the array
    SwitchPoint *list;      // array of switch points. When drawing to the buffer, we can just append. Before rendering, this must be sorted by abs(pos)
    void *p_heap, *r_heap;  // internal heaps for depth sorting
} ScanBuffer;

ScanBuffer *InitScanBuffer(int width, int height);

void FreeScanBuffer(ScanBuffer *buf);

// Fill a triagle with a solid colour
// Triangle must be clockwise winding
void FillTrangle(
    ScanBuffer *buf,
    int x0, int y0, 
    int x1, int y1,
    int x2, int y2,
    int z,
    int r, int g, int b);

void SetBackground(
    ScanBuffer *buf,
    int z, // depth of the background. Anything behind this will be invisible
    int r, int g, int b);

// Reset all drawing operations in the buffer, ready for next frame
// Do this *after* rendering to pixel buffer
void ClearScanBuffer(ScanBuffer *buf);

// Render a scan buffer to a pixel framebuffer
// This can be done on a different processor core from other draw commands to spread the load
// Do not draw to a buffer while it is rendering (switch buffers if you need to)
void RenderBuffer(
    ScanBuffer *buf,             // source scan buffer
    BYTE* data, int rowBytes,    // target frame-buffer
    int bufSize                  // size of target buffer
);


