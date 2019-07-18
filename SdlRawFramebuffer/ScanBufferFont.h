#pragma once

#ifndef scanbufferfont_h
#define scanbufferfont_h

#include "ScanBufferDraw.h"

// Write a glyph at the given position (y is baseline)
void AddGlyph(ScanBuffer *buf, char c, int x, int y, int z, uint32_t color);

#endif
