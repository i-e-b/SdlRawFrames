
#include "ImmediateDraw.h"

void CoverageLine(
    BYTE* data, int rowBytes,        // target buffer
    int x0, int y0, int x1, int y1,  // line coords
    BYTE r, BYTE g, BYTE b           // draw color
)
{
    int dx = x1 - x0, sx = (dx < 0) ? -1 : 1;
    int dy = y1 - y0, sy = (dy < 0) ? -1 : 1;

    // dx and dy always positive. sx & sy hold the sign
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    
    // adjust for the 1-pixel offset our paired-pixels approximation gives
    if (sy < 0) { y0++; y1++; }
    if (sx < 0) { x0++; x1++; }

    int pixoff; // target pixel as byte offset from base

    int pairoff = (dx > dy ? rowBytes : 4);  // paired pixel for AA, as byte offset from main pixel.

    int coverAdj = (dx + dy) / 2;            // to adjust `err` so it's centred over 0
    int err = 0;                             // running error
    int ds = (dx >= dy ? -sy : sx);          // error sign
    int errOff = (dx > dy ? dx + dy : 0);    // error adjustment

    for (;;) {
        // rough approximation of coverage, based on error
        int v = (err + coverAdj - errOff) * ds;
        if (v > 127) v = 127;
        if (v < -127) v = -127;
        int lv = 128 + v; // 'left' coverage,  0..255
        int rv = 128 - v; // 'right' coverage, 0..255

        // set primary pixel, mixing original colour with target colour
        pixoff = (y0 * rowBytes) + (x0 * 4);

        //                 [ existing colour mix        ]   [ line colour mix ]
        data[pixoff + 0] = ((data[pixoff + 0] * lv) >> 8) + ((r * rv) >> 8);
        data[pixoff + 1] = ((data[pixoff + 1] * lv) >> 8) + ((g * rv) >> 8);
        data[pixoff + 2] = ((data[pixoff + 2] * lv) >> 8) + ((b * rv) >> 8);

        pixoff += pairoff; // switch to the 'other' pixel

        data[pixoff + 0] = ((data[pixoff + 0] * rv) >> 8) + ((r * lv) >> 8);
        data[pixoff + 1] = ((data[pixoff + 1] * rv) >> 8) + ((g * lv) >> 8);
        data[pixoff + 2] = ((data[pixoff + 2] * rv) >> 8) + ((b * lv) >> 8);


        // end of line check
        if (x0 == x1 && y0 == y1) break;

        int e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}



void BresenhamLine(BYTE* data, int rowBytes, int x0, int y0, int x1, int y1)
{
    int dx = x1 - x0, sx = x0 < x1 ? 1 : -1;
    int dy = y1 - y0, sy = y0 < y1 ? 1 : -1;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;

    int e2, err = (dx > dy ? dx : -dy) / 2;
    int pixoff = 0;

    for (;;) {
        // set pixel (hard coded black for now)
        pixoff = (y0 * rowBytes) + (x0 * 4);
        data[pixoff] = 0;
        data[pixoff + 1] = 0;
        data[pixoff + 2] = 0;

        // end of line check
        if (x0 == x1 && y0 == y1) break;

        e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

