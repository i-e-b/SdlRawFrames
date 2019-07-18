#include "ScanBufferFont.h"

// TODO:
// Change the structures below. Every character has 8 rows. There aren't many unique rows
// so have a big data blob to handle that.




// Row-based components of the font
#define fEND 0xffff // end of glyph
// ON   0123456
// OFF  1234567
#define xx___xx(y) 0x##y##00, 0x##y##02, 0x##y##05, 0x##y##07,
#define _xxxxxx(y) 0x##y##01, 0x##y##07,
#define xxxxxxx(y) 0x##y##00, 0x##y##07,
#define xxxxxx_(y) 0x##y##00, 0x##y##06,
#define xxxxx__(y) 0x##y##00, 0x##y##05,
#define _xx_xx_(y) 0x##y##01, 0x##y##03, 0x##y##04, 0x##y##06,
#define __xxx__(y) 0x##y##02, 0x##y##05,
#define _xxxxx_(y) 0x##y##01, 0x##y##06,
#define xx_____(y) 0x##y##00, 0x##y##02,
#define _xx____(y) 0x##y##01, 0x##y##03,
#define xx__xx_(y) 0x##y##00, 0x##y##02, 0x##y##04, 0x##y##06,
#define xx__xxx(y) 0x##y##00, 0x##y##02, 0x##y##04, 0x##y##07,
#define xx_xxxx(y) 0x##y##00, 0x##y##02, 0x##y##03, 0x##y##07,
#define xx_xxx_(y) 0x##y##00, 0x##y##02, 0x##y##03, 0x##y##06,
#define xxx_xxx(y) 0x##y##00, 0x##y##03, 0x##y##04, 0x##y##07,
#define xxx__xx(y) 0x##y##00, 0x##y##03, 0x##y##05, 0x##y##07,
#define xxxx_xx(y) 0x##y##00, 0x##y##04, 0x##y##05, 0x##y##07,
#define _xxx_xx(y) 0x##y##01, 0x##y##04, 0x##y##05, 0x##y##07,
#define xx_x_xx(y) 0x##y##00, 0x##y##02, 0x##y##03, 0x##y##04, 0x##y##05, 0x##y##07,
#define ___x___(y) 0x##y##03, 0x##y##04,
#define __xxxx_(y) 0x##y##02, 0x##y##06,
#define ___xx__(y) 0x##y##03, 0x##y##05,
#define ____xx_(y) 0x##y##04, 0x##y##06,
#define _____xx(y) 0x##y##05, 0x##y##07,
#define _______(y)  

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

uint16_t ch_D[] = { xxxxxx_(06)
                    xx___xx(05)
                    xx___xx(04)
                    xx___xx(03)
                    xx___xx(02)
                    xx___xx(01)
                    xxxxxx_(00) fEND };

uint16_t ch_E[] = { xxxxxxx(06)
                    xx_____(05)
                    xx_____(04)
                    xxxxxx_(03)
                    xx_____(02)
                    xx_____(01)
                    xxxxxxx(00) fEND };

uint16_t ch_F[] = { xxxxxxx(06)
                    xx_____(05)
                    xx_____(04)
                    xxxxxx_(03)
                    xx_____(02)
                    xx_____(01)
                    xx_____(00) fEND };

uint16_t ch_G[] = { _xxxxx_(06)
                    xx___xx(05)
                    xx_____(04)
                    xx__xxx(03)
                    xx___xx(02)
                    xx___xx(01)
                    _xxxxx_(00) fEND };

uint16_t ch_H[] = { xx___xx(06)
                    xx___xx(05)
                    xx___xx(04)
                    xxxxxxx(03)
                    xx___xx(02)
                    xx___xx(01)
                    xx___xx(00) fEND };

uint16_t ch_I[] = { __xxxx_(06)
                    ___xx__(05)
                    ___xx__(04)
                    ___xx__(03)
                    ___xx__(02)
                    ___xx__(01)
                    __xxxx_(00) fEND };

uint16_t ch_J[] = { _____xx(06)
                    _____xx(05)
                    _____xx(04)
                    _____xx(03)
                    _____xx(02)
                    xx___xx(01)
                    _xxxxx_(00) fEND };

uint16_t ch_K[] = { xx___xx(06)
                    xx___xx(05)
                    xx__xx_(04)
                    xxxxx__(03)
                    xx__xx_(02)
                    xx___xx(01)
                    xx___xx(00) fEND };

uint16_t ch_L[] = { xx_____(06)
                    xx_____(05)
                    xx_____(04)
                    xx_____(03)
                    xx_____(02)
                    xx_____(01)
                    xxxxxxx(00) fEND };

uint16_t ch_M[] = { xx___xx(06)
                    xxx_xxx(05)
                    xxxxxxx(04)
                    xx_x_xx(03)
                    xx___xx(02)
                    xx___xx(01)
                    xx___xx(00) fEND };

uint16_t ch_N[] = { xx___xx(06)
                    xxx__xx(05)
                    xxxx_xx(04)
                    xxxxxxx(03)
                    xx_xxxx(02)
                    xx__xxx(01)
                    xx___xx(00) fEND };

uint16_t ch_O[] = { _xxxxx_(06)
                    xx___xx(05)
                    xx___xx(04)
                    xx___xx(03)
                    xx___xx(02)
                    xx___xx(01)
                    _xxxxx_(00) fEND };

uint16_t ch_P[] = { xxxxxx_(06)
                    xx___xx(05)
                    xx___xx(04)
                    xx___xx(03)
                    xxxxxx_(02)
                    xx_____(01)
                    xx_____(00) fEND };

uint16_t ch_Q[] = { _xxxxx_(06)
                    xx___xx(05)
                    xx___xx(04)
                    xx___xx(03)
                    xx_xxx_(02)
                    xx__xxx(01)
                    _xxx_xx(00) fEND };

uint16_t ch_R[] = { xxxxxx_(06)
                    xx___xx(05)
                    xx___xx(04)
                    xx__xx_(03)
                    xxxxx__(02)
                    xx_xxx_(01)
                    xx__xxx(00) fEND };

uint16_t ch_S[] = { _xxxxx_(06)
                    xx___xx(05)
                    xx_____(04)
                    _xxxxx_(03)
                    _____xx(02)
                    xx___xx(01)
                    _xxxxx_(00) fEND };

uint16_t ch_T[] = { _xxxxxx(06)
                    ___xx__(05)
                    ___xx__(04)
                    ___xx__(03)
                    ___xx__(02)
                    ___xx__(01)
                    ___xx__(00) fEND };

uint16_t ch_U[] = { xx___xx(06)
                    xx___xx(05)
                    xx___xx(04)
                    xx___xx(03)
                    xx___xx(02)
                    xx___xx(01)
                    _xxxxx_(00) fEND };

uint16_t ch_V[] = { xx___xx(06)
                    xx___xx(05)
                    xx___xx(04)
                    xx___xx(03)
                    _xx_xx_(02)
                    __xxx__(01)
                    ___x___(00) fEND };

uint16_t ch_W[] = { xx___xx(06)
                    xx___xx(05)
                    xx___xx(04)
                    xx_x_xx(03)
                    xxxxxxx(02)
                    xxx_xxx(01)
                    xx___xx(00) fEND };

uint16_t ch_X[] = { xx___xx(06)
                    xx___xx(05)
                    _xx_xx_(04)
                    __xxx__(03)
                    _xx_xx_(02)
                    xx___xx(01)
                    xx___xx(00) fEND };

uint16_t ch_Y[] = { xx___xx(06)
                    xx___xx(05)
                    _xx_xx_(04)
                    __xxx__(03)
                    ___xx__(02)
                    ___xx__(01)
                    ___xx__(00) fEND };

uint16_t ch_Z[] = { xxxxxxx(06)
                    _____xx(05)
                    ____xx_(04)
                    __xxx__(03)
                    _xx____(02)
                    xx_____(01)
                    xxxxxxx(00) fEND };

uint16_t Bang[] = { ___xx__(06)
                    __xxxx_(05)
                    __xxxx_(04)
                    ___xx__(03)
                    _______(02)
                    ___xx__(01)
                    ___xx__(00) fEND };

// Invalid char, control char, or not yet mapped
uint16_t chIV[] = { xxxxxxx(06)
                    _______(05)
                    xxxxxxx(04)
                    _______(03)
                    xxxxxxx(02)
                    _______(01)
                    xxxxxxx(00) fEND };

uint16_t* charMap[] { // ASCII, starting from 33 (0x21) '!'
 Bang, chIV, chIV, chIV, chIV, chIV, chIV, chIV, chIV, chIV, // PUNCT & NUM
 chIV, chIV, chIV, chIV, chIV, chIV, chIV, chIV, chIV, chIV,
 chIV, chIV, chIV, chIV, chIV, chIV, chIV, chIV, chIV, chIV,
 chIV, chIV, ch_A, ch_B, ch_C, ch_D, ch_E, ch_F, ch_G, ch_H, // UPPER CASE
 ch_I, ch_J, ch_K, ch_L, ch_M, ch_N, ch_O, ch_P, ch_Q, ch_R,
 ch_S, ch_T, ch_U, ch_V, ch_W, ch_X, ch_Y, ch_Z, chIV, chIV,
 chIV, chIV, chIV, chIV, ch_A, ch_B, ch_C, ch_D, ch_E, ch_F, // LOWER CASE
 ch_G, ch_H, ch_I, ch_J, ch_K, ch_L, ch_M, ch_N, ch_O, ch_P,
 ch_Q, ch_R, ch_S, ch_T, ch_U, ch_V, ch_W, ch_X, ch_Y, ch_Z,
 chIV, chIV, chIV, chIV, chIV
};

// Set a point with an exact position, clipped to bounds
// SetSP(ScanBuffer * buf, int x, int y, uint16_t objectId, uint8_t isOn);

// Set or update material values for an object
// SetMaterial(ScanBuffer* buf, uint16_t objectId, int depth, uint32_t color);

#define SAFETY_LIMIT 50

void AddGlyph(ScanBuffer *buf, char c, int x, int y, int z, uint32_t color) {
    if (buf == NULL) return;
    if (c < 33 || c > 126) return;

    // pick a char block. For now, just use 'A'
    uint16_t* points = charMap[c - 33];

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