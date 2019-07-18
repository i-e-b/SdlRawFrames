#include "ScanBufferFont.h"

// Row-based components of the font
#define fEND 0xffff // end of glyph
#define xx___xx(y) 0x##y##00, 0x##y##02, 0x##y##06, 0x##y##08,
#define xxxxxxx(y) 0x##y##00, 0x##y##08,
#define xxxxxx_(y) 0x##y##00, 0x##y##07,
#define _xx_xx_(y) 0x##y##01, 0x##y##03, 0x##y##05, 0x##y##07,
#define __xxx__(y) 0x##y##02, 0x##y##06,
#define _xxxxx_(y) 0x##y##01, 0x##y##07,
#define xx_____(y) 0x##y##00, 0x##y##02,

// Font data (hard coded basic 8x8)
// pairs of ON & OFF; 1st byte is row, 2nd byte is xpos. 0xFFFF marks the end
// row is upside-down, treat as negative offset from baseline.
uint16_t ch_A[] = { __xxx__(06)
                    _xx_xx_(05)
                    xx___xx(04)
                    xx___xx(03)
                    xxxxxxx(02)
                    xx___xx(01)
                    xx___xx(00) fEND };

uint16_t ch_B[] = { xxxxxx_(06)
                    xx___xx(05)
                    xx___xx(04)
                    xxxxxx_(03)
                    xx___xx(02)
                    xx___xx(01)
                    xxxxxx_(00) fEND };

uint16_t ch_C[] = { _xxxxx_(06)
                    xx___xx(05)
                    xx_____(04)
                    xx_____(03)
                    xx_____(02)
                    xx___xx(01)
                    _xxxxx_(00) fEND };

// Set a point with an exact position, clipped to bounds
// SetSP(ScanBuffer * buf, int x, int y, uint16_t objectId, uint8_t isOn);

// Set or update material values for an object
// SetMaterial(ScanBuffer* buf, uint16_t objectId, int depth, uint32_t color);

#define SAFETY_LIMIT 50

void AddGlyph(ScanBuffer *buf, char c, int x, int y, int z, uint32_t color) {
    if (buf == NULL) return;
    if (c < 32 || c > 126) return;

    // pick a char block. For now, just use 'A'
    uint16_t* points;
    switch (c) {
    case 'A': points = ch_A; break;
    case 'B': points = ch_B; break;
    case 'C': points = ch_C; break;

    default: return; // don't bother drawing
    }


    // set objectId, color, and depth
    int objId = buf->itemCount;
    buf->itemCount ++;
    SetMaterial(buf, objId, z, color);

    // draw points
    bool on = true;
    for (int i = 0; i < SAFETY_LIMIT; i++) {
        if (points[i] == 0xFFFF) break;
        int px = x + (points[i] & 0xff);
        int py = y - (points[i] >> 8);
        SetSP(buf, px, py, objId, on);
        on = !on;
    }
}