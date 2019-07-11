#pragma once
#include <stdint.h>

#ifndef BYTE
#define BYTE unsigned char
#endif

// Functions to use a scan buffer
// for rendering filled shapes 


// entry for each 'pixel' in a scan buffer
// 'drawing' involves writing a list of these, sorting by x-position, then filling the scanline
// Notes: 1080p resolution is 1920x1080 = 2'073'600 pixels. 2^22 is 4'194'304; 2^21 -1 = 2'097'151
// Using per-row buffers, we only need 2048, or about 11 bits
typedef struct SwitchPoint {
    uint32_t xpos:11;       // position of switch-point, as (y*width)+x; (21 bits left)
    uint32_t id:16;         // the object ID (used for material lookup, 65k limit) (5 bits left)
    uint32_t state:1;       // 1 = 'on' point, 0 = 'off' point.
    uint32_t reserved:4;    // not yet used.
} SwitchPoint;

typedef struct Material {
    uint32_t color;         // RGB (24 bit of color).
    int16_t  depth;         // z-position in final image
} Material;

typedef struct ScanLine {
    int32_t count;          // number of items in the array
    int32_t length;         // memory length of the array

    SwitchPoint* points;    // When drawing to the buffer, we can just append. Before rendering, this must be sorted by xpos
} ScanLine;

// buffer of switch points.
typedef struct ScanBuffer {
    uint16_t itemCount;     // used to give each switch-point a unique ID. This is critical for the depth-sorting process
    int height;
    int width;
    int32_t expectedScanBufferSize;

    ScanLine* scanLines;    // matrix of switch points.
    Material* materials;    // draw properties for each object
    void *p_heap, *r_heap;  // internal heaps for depth sorting
} ScanBuffer;

ScanBuffer *InitScanBuffer(int width, int height);

void FreeScanBuffer(ScanBuffer *buf);

// Fill a triagle with a solid colour
void FillTrangle( ScanBuffer *buf,
    int x0, int y0, 
    int x1, int y1,
    int x2, int y2,
    int z,
    int r, int g, int b);

// Fill an axis aligned rectangle
void FillRect(ScanBuffer *buf,
    int left, int top, int right, int bottom,
    int z,
    int r, int g, int b);

void FillCircle(ScanBuffer *buf,
    int x, int y, int radius,
    int z,
    int r, int g, int b);

void FillEllipse(ScanBuffer *buf,
    int xc, int yc, int width, int height,
    int z,
    int r, int g, int b);

// Fill a quad given 3 points
void FillTriQuad(ScanBuffer *buf,
    int x0, int y0,
    int x1, int y1,
    int x2, int y2,
    int z,
    int r, int g, int b);

// draw a line with width
void DrawLine(ScanBuffer *buf,
    int x0, int y0,
    int x1, int y1,
    int z, int w, // width
    int r, int g, int b);

// draw the border of an ellipse
void OutlineEllipse(ScanBuffer *buf,
    int xc, int yc, int width, int height,
    int z, int w, // outline width
    int r, int g, int b);

// Set a background plane
void SetBackground( ScanBuffer *buf,
    int z, // depth of the background. Anything behind this will be invisible
    int r, int g, int b);

// draw everywhere except in the ellipse
void EllipseHole(ScanBuffer *buf,
    int xc, int yc, int width, int height,
    int z,
    int r, int g, int b);

// Reset all drawing operations in the buffer, ready for next frame
// Do this *after* rendering to pixel buffer
void ClearScanBuffer(ScanBuffer *buf);

// Render a scan buffer to a pixel framebuffer
// This can be done on a different processor core from other draw commands to spread the load
// Do not draw to a scan buffer while it is rendering (switch buffers if you need to)
void RenderBuffer(
    ScanBuffer *buf, // source scan buffer
    BYTE* data       // target frame-buffer (must match scanbuffer dimensions)
);


