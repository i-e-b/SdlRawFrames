#pragma once

#ifndef BYTE
#define BYTE unsigned char
#endif

// Functions for drawing to the frame buffer directly

// Antialised

void CoverageLine(                   // Draw an antialiased line
    BYTE* data, int rowBytes,        // target buffer
    int x0, int y0, int x1, int y1,  // line coords
    BYTE r, BYTE g, BYTE b           // draw color
);


// Aliased

void BresenhamLine(BYTE* data, int rowBytes, int x0, int y0, int x1, int y1);