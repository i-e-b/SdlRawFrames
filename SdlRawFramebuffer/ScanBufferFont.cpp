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
#define _xxx___(y) 0x##y##01, 0x##y##04,
#define __x_x__(y) 0x##y##02, 0x##y##03, 0x##y##04, 0x##y##05,
#define _xx_x__(y) 0x##y##01, 0x##y##03, 0x##y##04, 0x##y##05,
#define _xx_xx_(y) 0x##y##01, 0x##y##03, 0x##y##04, 0x##y##06,
#define _xx__xx(y) 0x##y##01, 0x##y##03, 0x##y##05, 0x##y##07,
#define __xx__x(y) 0x##y##02, 0x##y##04, 0x##y##06, 0x##y##07,
#define xx_xx__(y) 0x##y##00, 0x##y##02, 0x##y##03, 0x##y##05,
#define __xxx__(y) 0x##y##02, 0x##y##05,
#define _xxxxx_(y) 0x##y##01, 0x##y##06,
#define xx_____(y) 0x##y##00, 0x##y##02,
#define _xx____(y) 0x##y##01, 0x##y##03,
#define __xx___(y) 0x##y##02, 0x##y##04,
#define ___xx__(y) 0x##y##03, 0x##y##05,
#define ____xx_(y) 0x##y##04, 0x##y##06,
#define _____xx(y) 0x##y##05, 0x##y##07,
#define xx__xx_(y) 0x##y##00, 0x##y##02, 0x##y##04, 0x##y##06,
#define xxx_xx_(y) 0x##y##00, 0x##y##03, 0x##y##04, 0x##y##06,
#define xx__xxx(y) 0x##y##00, 0x##y##02, 0x##y##04, 0x##y##07,
#define xx_xxxx(y) 0x##y##00, 0x##y##02, 0x##y##03, 0x##y##07,
#define x_xxxx_(y) 0x##y##00, 0x##y##01, 0x##y##02, 0x##y##06,
#define x_xxx__(y) 0x##y##00, 0x##y##01, 0x##y##02, 0x##y##05,
#define xx_xxx_(y) 0x##y##00, 0x##y##02, 0x##y##03, 0x##y##06,
#define xxx_xxx(y) 0x##y##00, 0x##y##03, 0x##y##04, 0x##y##07,
#define xxx__xx(y) 0x##y##00, 0x##y##03, 0x##y##05, 0x##y##07,
#define xxxx_xx(y) 0x##y##00, 0x##y##04, 0x##y##05, 0x##y##07,
#define _xxx_xx(y) 0x##y##01, 0x##y##04, 0x##y##05, 0x##y##07,
#define _xxxx_x(y) 0x##y##01, 0x##y##05, 0x##y##06, 0x##y##07,
#define xx_x_xx(y) 0x##y##00, 0x##y##02, 0x##y##03, 0x##y##04, 0x##y##05, 0x##y##07,
#define ___x___(y) 0x##y##03, 0x##y##04,
#define __x____(y) 0x##y##02, 0x##y##03,
#define __xxxx_(y) 0x##y##02, 0x##y##06,
#define ____xxx(y) 0x##y##04, 0x##y##07,
#define _______(y)  

// Font data (hard coded basic 8x8)
// pairs of ON & OFF; 1st byte is row, 2nd byte is xpos. 0xFFFF marks the end
// row is upside-down, treat as negative offset from baseline.
uint16_t ch_A[] = { __xxx__(07)
                    _xx_xx_(06)
                    xx___xx(05)
                    xx___xx(04)
                    xxxxxxx(03)
                    xx___xx(02)
                    xx___xx(01) fEND };

uint16_t ch_a[] = { _______(07)
                    _______(06)
                    _xxxxx_(05)
                    _____xx(04)
                    _xxxxxx(03)
                    xx___xx(02)
                    _xxxx_x(01) fEND };

uint16_t ch_B[] = { xxxxxx_(07)
                    xx___xx(06)
                    xx___xx(05)
                    xxxxxx_(04)
                    xx___xx(03)
                    xx___xx(02)
                    xxxxxx_(01) fEND };

uint16_t ch_b[] = { xx_____(07)
                    xx_____(06)
                    xxxxxx_(05)
                    xx___xx(04)
                    xx___xx(03)
                    xxx__xx(02)
                    xx_xxx_(01) fEND };

uint16_t ch_C[] = { _xxxxx_(07)
                    xx___xx(06)
                    xx_____(05)
                    xx_____(04)
                    xx_____(03)
                    xx___xx(02)
                    _xxxxx_(01) fEND };

uint16_t ch_c[] = { _______(07)
                    _______(06)
                    _xxxxx_(05)
                    xx___xx(04)
                    xx_____(03)
                    xx___xx(02)
                    _xxxxx_(01) fEND };

uint16_t ch_D[] = { xxxxxx_(07)
                    xx___xx(06)
                    xx___xx(05)
                    xx___xx(04)
                    xx___xx(03)
                    xx___xx(02)
                    xxxxxx_(01) fEND };

uint16_t ch_d[] = { _____xx(07)
                    _____xx(06)
                    _xxxxxx(05)
                    xx___xx(04)
                    xx___xx(03)
                    xx__xxx(02)
                    _xxxx_x(01) fEND };

uint16_t ch_E[] = { xxxxxxx(07)
                    xx_____(06)
                    xx_____(05)
                    xxxxxx_(04)
                    xx_____(03)
                    xx_____(02)
                    xxxxxxx(01) fEND };

uint16_t ch_e[] = { _______(07)
                    _______(06)
                    _xxxxx_(05)
                    xx___xx(04)
                    xxxxxxx(03)
                    xx_____(02)
                    _xxxxxx(01) fEND };

uint16_t ch_F[] = { xxxxxxx(07)
                    xx_____(06)
                    xx_____(05)
                    xxxxxx_(04)
                    xx_____(03)
                    xx_____(02)
                    xx_____(01) fEND };

uint16_t ch_f[] = { ____xxx(07)
                    ___xx__(06)
                    _xxxxxx(05)
                    ___xx__(04)
                    ___xx__(03)
                    ___xx__(02)
                    ___xx__(01) fEND };

uint16_t ch_G[] = { _xxxxx_(07)
                    xx___xx(06)
                    xx_____(05)
                    xx__xxx(04)
                    xx___xx(03)
                    xx___xx(02)
                    _xxxxx_(01) fEND };

uint16_t ch_g[] = { _______(06)
                    _xxxx_x(05)
                    xx___xx(04)
                    xx___xx(03)
                    _xxxxxx(02)
                    _____xx(01)
                    _xxxxx_(00) fEND };

uint16_t ch_H[] = { xx___xx(07)
                    xx___xx(06)
                    xx___xx(05)
                    xxxxxxx(04)
                    xx___xx(03)
                    xx___xx(02)
                    xx___xx(01) fEND };

uint16_t ch_h[] = { xx_____(07)
                    xx_____(06)
                    xxxxxx_(05)
                    xx___xx(04)
                    xx___xx(03)
                    xx___xx(02)
                    xx___xx(01) fEND };

uint16_t ch_I[] = { __xxxx_(07)
                    ___xx__(06)
                    ___xx__(05)
                    ___xx__(04)
                    ___xx__(03)
                    ___xx__(02)
                    __xxxx_(01) fEND };

uint16_t ch_i[] = { ___xx__(07)
                    _______(06)
                    __xxx__(05)
                    ___xx__(04)
                    ___xx__(03)
                    ___xx__(02)
                    ___xx__(01) fEND };

uint16_t ch_J[] = { _____xx(07)
                    _____xx(06)
                    _____xx(05)
                    _____xx(04)
                    _____xx(03)
                    xx___xx(02)
                    _xxxxx_(01) fEND };

uint16_t ch_j[] = { ___xx__(07)
                    _______(06)
                    __xxx__(05)
                    ___xx__(04)
                    ___xx__(03)
                    ___xx__(02)
                    xx_xx__(01)
                    _xxx___(00)  fEND };

uint16_t ch_K[] = { xx___xx(07)
                    xx___xx(06)
                    xx__xx_(05)
                    xxxxx__(04)
                    xx__xx_(03)
                    xx___xx(02)
                    xx___xx(01) fEND };

uint16_t ch_k[] = { xx_____(07)
                    xx_____(06)
                    xx___xx(05)
                    xx__xx_(04)
                    xxxxx__(03)
                    xx__xx_(02)
                    xx___xx(01) fEND };

uint16_t ch_L[] = { xx_____(07)
                    xx_____(06)
                    xx_____(05)
                    xx_____(04)
                    xx_____(03)
                    xx_____(02)
                    xxxxxxx(01) fEND };

uint16_t ch_l[] = { __xxx__(07)
                    ___xx__(06)
                    ___xx__(05)
                    ___xx__(04)
                    ___xx__(03)
                    ___xx__(02)
                    ____xxx(01) fEND };

uint16_t ch_M[] = { xx___xx(07)
                    xxx_xxx(06)
                    xxxxxxx(05)
                    xx_x_xx(04)
                    xx___xx(03)
                    xx___xx(02)
                    xx___xx(01) fEND };

uint16_t ch_m[] = { _______(07)
                    _______(06)
                    _xx_xx_(05)
                    xx_x_xx(04)
                    xx_x_xx(03)
                    xx_x_xx(02)
                    xx_x_xx(01) fEND };

uint16_t ch_N[] = { xx___xx(07)
                    xxx__xx(06)
                    xxxx_xx(05)
                    xxxxxxx(04)
                    xx_xxxx(03)
                    xx__xxx(02)
                    xx___xx(01) fEND };

uint16_t ch_n[] = { _______(07)
                    _______(06)
                    xx_xx__(05)
                    xxx_xx_(04)
                    xx___xx(03)
                    xx___xx(02)
                    xx___xx(01) fEND };

uint16_t ch_O[] = { _xxxxx_(07)
                    xx___xx(06)
                    xx___xx(05)
                    xx___xx(04)
                    xx___xx(03)
                    xx___xx(02)
                    _xxxxx_(01) fEND };

uint16_t ch_o[] = { _______(07)
                    _______(06)
                    _xxxxx_(05)
                    xx___xx(04)
                    xx___xx(03)
                    xx___xx(02)
                    _xxxxx_(01) fEND };

uint16_t ch_P[] = { xxxxxx_(07)
                    xx___xx(06)
                    xx___xx(05)
                    xx___xx(04)
                    xxxxxx_(03)
                    xx_____(02)
                    xx_____(01) fEND };

uint16_t ch_p[] = { _______(06)
                    x_xxxx_(05)
                    xx___xx(04)
                    xx___xx(03)
                    xxxxxx_(02)
                    xx_____(01)
                    xx_____(00) fEND };

uint16_t ch_Q[] = { _xxxxx_(07)
                    xx___xx(06)
                    xx___xx(05)
                    xx___xx(04)
                    xx_xxx_(03)
                    xx__xxx(02)
                    _xxx_xx(01) fEND };

uint16_t ch_q[] = { _______(06)
                    _xxxx_x(05)
                    xx___xx(04)
                    xx___xx(03)
                    _xxxxxx(02)
                    _____xx(01)
                    _____xx(00) fEND };

uint16_t ch_R[] = { xxxxxx_(07)
                    xx___xx(06)
                    xx___xx(05)
                    xx__xx_(04)
                    xxxxx__(03)
                    xx_xxx_(02)
                    xx__xxx(01) fEND };

uint16_t ch_r[] = { _______(07)
                    _______(06)
                    x_xxx__(05)
                    xx__xx_(04)
                    xx_____(03)
                    xx_____(02)
                    xx_____(01) fEND };

uint16_t ch_S[] = { _xxxxx_(07)
                    xx___xx(06)
                    xx_____(05)
                    _xxxxx_(04)
                    _____xx(03)
                    xx___xx(02)
                    _xxxxx_(01) fEND };

uint16_t ch_s[] = { _______(07)
                    _______(06)
                    _xxxxx_(05)
                    xx_____(04)
                    _xxxxx_(03)
                    _____xx(02)
                    _xxxxx_(01) fEND };

uint16_t ch_T[] = { _xxxxxx(07)
                    ___xx__(06)
                    ___xx__(05)
                    ___xx__(04)
                    ___xx__(03)
                    ___xx__(02)
                    ___xx__(01) fEND };

uint16_t ch_t[] = { _______(07)
                    xx_____(06)
                    xxxxx__(05)
                    xx_____(04)
                    xx_____(03)
                    xx___xx(02)
                    _xxxxx_(01) fEND };

uint16_t ch_U[] = { xx___xx(07)
                    xx___xx(06)
                    xx___xx(05)
                    xx___xx(04)
                    xx___xx(03)
                    xx___xx(02)
                    _xxxxx_(01) fEND };

uint16_t ch_u[] = { _______(07)
                    _______(06)
                    xx___xx(05)
                    xx___xx(04)
                    xx___xx(03)
                    xx___xx(02)
                    _xxxx_x(01) fEND };

uint16_t ch_V[] = { xx___xx(07)
                    xx___xx(06)
                    xx___xx(05)
                    xx___xx(04)
                    _xx_xx_(03)
                    __xxx__(02)
                    ___x___(01) fEND };

uint16_t ch_v[] = { _______(07)
                    _______(06)
                    xx___xx(05)
                    xx___xx(04)
                    _xx_xx_(03)
                    __xxx__(02)
                    ___x___(01) fEND };

uint16_t ch_W[] = { xx___xx(07)
                    xx___xx(06)
                    xx___xx(05)
                    xx_x_xx(04)
                    xxxxxxx(03)
                    xxx_xxx(02)
                    xx___xx(01) fEND };

uint16_t ch_w[] = { _______(07)
                    _______(06)
                    xx_x_xx(05)
                    xx_x_xx(04)
                    xx_x_xx(03)
                    xx_x_xx(02)
                    _xx_xx_(01) fEND };

uint16_t ch_X[] = { xx___xx(07)
                    xx___xx(06)
                    _xx_xx_(05)
                    __xxx__(04)
                    _xx_xx_(03)
                    xx___xx(02)
                    xx___xx(01) fEND };

uint16_t ch_x[] = { _______(07)
                    _______(06)
                    xx___xx(05)
                    _xx_xx_(04)
                    __xxx__(03)
                    _xx_xx_(02)
                    xx___xx(01) fEND };

uint16_t ch_Y[] = { xx___xx(07)
                    xx___xx(06)
                    _xx_xx_(05)
                    __xxx__(04)
                    ___xx__(03)
                    ___xx__(02)
                    ___xx__(01) fEND };

uint16_t ch_y[] = { _______(06)
                    xx___xx(05)
                    xx___xx(04)
                    xx___xx(03)
                    _xxxxxx(02)
                    _____xx(01)
                    _xxxxx_(00) fEND };

uint16_t ch_Z[] = { xxxxxxx(07)
                    _____xx(06)
                    ____xx_(05)
                    __xxx__(04)
                    _xx____(03)
                    xx_____(02)
                    xxxxxxx(01) fEND };

uint16_t ch_z[] = { _______(07)
                    _______(06)
                    xxxxxxx(05)
                    ____xx_(04)
                    __xxx__(03)
                    _xx____(02)
                    xxxxxxx(01) fEND };

uint16_t Bang[] = { ___xx__(07)
                    __xxxx_(06)
                    __xxxx_(05)
                    ___xx__(04)
                    _______(03)
                    ___xx__(02)
                    ___xx__(01) fEND };

uint16_t ch_0[] = { _xxxxx_(07)
                    xx___xx(06)
                    xx__xxx(05)
                    xx_x_xx(04)
                    xxx__xx(03)
                    xx___xx(02)
                    _xxxxx_(01) fEND };

uint16_t ch_1[] = { ___xx__(07)
                    xxxxx__(06)
                    ___xx__(05)
                    ___xx__(04)
                    ___xx__(03)
                    ___xx__(02)
                    _xxxxxx(01) fEND };

uint16_t ch_2[] = { _xxxxx_(07)
                    xx___xx(06)
                    ____xx_(05)
                    ___xx__(04)
                    __xx___(03)
                    _xx____(02)
                    xxxxxxx(01) fEND };

uint16_t ch_3[] = { _xxxxx_(07)
                    xx___xx(06)
                    _____xx(05)
                    __xxxx_(04)
                    _____xx(03)
                    xx___xx(02)
                    _xxxxx_(01) fEND };

uint16_t ch_4[] = { xx__xx_(07)
                    xx__xx_(06)
                    xx__xx_(05)
                    xx__xx_(04)
                    xxxxxxx(03)
                    ____xx_(02)
                    ____xx_(01) fEND };

uint16_t ch_5[] = { xxxxxxx(07)
                    xx_____(06)
                    xxxxxx_(05)
                    xx___xx(04)
                    _____xx(03)
                    xx___xx(02)
                    _xxxxx_(01) fEND };

uint16_t ch_6[] = { _xxxxx_(07)
                    xx___xx(06)
                    xx_____(05)
                    xxxxxx_(04)
                    xx___xx(03)
                    xx___xx(02)
                    _xxxxx_(01) fEND };

uint16_t ch_7[] = { xxxxxxx(07)
                    xx___xx(06)
                    ____xx_(05)
                    ___xx__(04)
                    __xx___(03)
                    __xx___(02)
                    __xx___(01) fEND };

uint16_t ch_8[] = { _xxxxx_(07)
                    xx___xx(06)
                    xx___xx(05)
                    _xxxxx_(04)
                    xx___xx(03)
                    xx___xx(02)
                    _xxxxx_(01) fEND };

uint16_t ch_9[] = { _xxxxx_(07)
                    xx___xx(06)
                    xx___xx(05)
                    _xxxxxx(04)
                    _____xx(03)
                    xx___xx(02)
                    _xxxxx_(01) fEND };

uint16_t cmma[] = { __xx___(03)
                    __xx___(02)
                    ___x___(01)
                    __x____(00) fEND };

uint16_t stop[] = { __xx___(02)
                    __xx___(01) fEND };

uint16_t dqot[] = { _xx_xx_(07)
                    _xx_xx_(06)
                    __x_x__(05) fEND };

uint16_t quot[] = { __xx___(07)
                    __xx___(06)
                    ___x___(05)
                    __x____(04) fEND };

uint16_t hash[] = { _xx_xx_(07)
                    xxxxxxx(06)
                    _xx_xx_(05)
                    _xx_xx_(04)
                    _xx_xx_(03)
                    xxxxxxx(02)
                    _xx_xx_(01) fEND };

uint16_t dola[] = { ___xx__(07)
                    _xxxxx_(06)
                    xx_____(05)
                    _xxxxx_(04)
                    _____xx(03)
                    _xxxxx_(02)
                    ___xx__(01) fEND };

uint16_t prcn[] = { xx___xx(07)
                    xx___xx(06)
                    ____xx_(05)
                    __xxx__(04)
                    _xx____(03)
                    xx___xx(02)
                    xx___xx(01) fEND };

uint16_t amps[] = { __xx___(07)
                    _xx_x__(06)
                    _xx_x__(05)
                    __xx__x(04)
                    xx_xxx_(03)
                    xx__xx_(02)
                    _xxx_xx(01) fEND };

uint16_t Lpar[] = { __xx___(07)
                    _xx____(06)
                    xx_____(05)
                    xx_____(04)
                    xx_____(03)
                    _xx____(02)
                    __xx___(01) fEND };

uint16_t Rpar[] = { __xx___(07)
                    ___xx__(06)
                    ____xx_(05)
                    ____xx_(04)
                    ____xx_(03)
                    ___xx__(02)
                    __xx___(01) fEND };


uint16_t Lbrk[] = { xxxxx__(07)
                    xx_____(06)
                    xx_____(05)
                    xx_____(04)
                    xx_____(03)
                    xx_____(02)
                    xxxxx__(01) fEND };

uint16_t Rbrk[] = { xxxxx__(07)
                    ___xx__(06)
                    ___xx__(05)
                    ___xx__(04)
                    ___xx__(03)
                    ___xx__(02)
                    xxxxx__(01) fEND };


uint16_t Lbce[] = { ____xx_(07)
                    ___xx__(06)
                    ___xx__(05)
                    _xxx___(04)
                    ___xx__(03)
                    ___xx__(02)
                    ____xx_(01) fEND };

uint16_t Rbce[] = { __xx___(07)
                    ___xx__(06)
                    ___xx__(05)
                    ____xxx(04)
                    ___xx__(03)
                    ___xx__(02)
                    __xx___(01) fEND };

uint16_t astr[] = { ___x___(07)
                    xx_x_xx(06)
                    __xxx__(05)
                    ___x___(04)
                    __xxx__(03)
                    xx_x_xx(02)
                    ___x___(01) fEND };

uint16_t plus[] = { ___x___(06)
                    ___x___(05)
                    _xxxxx_(04)
                    ___x___(03)
                    ___x___(02)fEND };

uint16_t negv[] = { _xxxxx_(04)fEND };

uint16_t fsla[] = { _____xx(06)
                    ____xx_(05)
                    ___xx__(04)
                    __xx___(03)
                    _xx____(02)
                    xx_____(01) fEND };

uint16_t bsla[] = { xx_____(07)
                    _xx____(06)
                    __xx___(05)
                    ___xx__(04)
                    ____xx_(03)
                    _____xx(02) fEND };

uint16_t coln[] = { __xx___(06)
                    __xx___(05)
                    _______(04)
                    _______(03)
                    __xx___(02)
                    __xx___(01) fEND };

uint16_t scln[] = { __xx___(06)
                    __xx___(05)
                    _______(04)
                    __xx___(03)
                    __xx___(02)
                    ___x___(01)
                    __x____(00) fEND };

uint16_t chLT[] = { ____xx_(06)
                    __xx___(05)
                    xx_____(04)
                    __xx___(03)
                    ____xx_(02) fEND };

uint16_t chGT[] = { xx_____(06)
                    __xx___(05)
                    ____xx_(04)
                    __xx___(03)
                    xx_____(02) fEND };

uint16_t equl[] = { xxxxxxx(05)
                    _______(04)
                    xxxxxxx(03) fEND };


uint16_t ques[] = { _xxxxx_(07)
                    xx___xx(06)
                    xx__xx_(05)
                    ___xx__(04)
                    __xx___(03)
                    _______(02)
                    __xx___(01) fEND };

uint16_t cmat[] = { _xxxxx_(07)
                    xx___xx(06)
                    xx__xxx(05)
                    xx_x_xx(04)
                    xx__xxx(03)
                    xx_____(02)
                    _xxxxx_(01) fEND };


uint16_t chev[] = { ___x___(07)
                    __xxx__(06)
                    _xx_xx_(05)
                    xx___xx(04) fEND };

uint16_t btik[] = { __xx___(07)
                    ___xx__(06)
                    ____xx_(05) fEND };

uint16_t uscr[] = { xxxxxxx(00) fEND };

uint16_t tild[] = { _xx__xx(05)
                    xx_x_xx(04)
                    xx__xx_(03) fEND };

uint16_t vert[] = { ___x___(07)
                    ___x___(06)
                    ___x___(05)
                    ___x___(04)
                    ___x___(03)
                    ___x___(02)
                    ___x___(01) fEND };

// Invalid char, control char, or not yet mapped
uint16_t ____[] = { xxxxxxx(07)
                    _______(06)
                    xxxxxxx(05)
                    _______(04)
                    xxxxxxx(03)
                    _______(02)
                    xxxxxxx(01) fEND };

uint16_t* charMap[] { // ASCII, starting from 33 (0x21) '!'
 Bang, dqot, hash, dola, prcn, amps, quot, Lpar, Rpar, astr, // PUNCT & NUM
 plus, cmma, negv, stop, fsla, ch_0, ch_1, ch_2, ch_3, ch_4,
 ch_5, ch_6, ch_7, ch_8, ch_9, coln, scln, chLT, equl, chGT,
 ques, cmat, ch_A, ch_B, ch_C, ch_D, ch_E, ch_F, ch_G, ch_H, // UPPER CASE
 ch_I, ch_J, ch_K, ch_L, ch_M, ch_N, ch_O, ch_P, ch_Q, ch_R,
 ch_S, ch_T, ch_U, ch_V, ch_W, ch_X, ch_Y, ch_Z, Lbrk, bsla,
 Rbrk, chev, uscr, btik, ch_a, ch_b, ch_c, ch_d, ch_e, ch_f, // LOWER CASE
 ch_g, ch_h, ch_i, ch_j, ch_k, ch_l, ch_m, ch_n, ch_o, ch_p,
 ch_q, ch_r, ch_s, ch_t, ch_u, ch_v, ch_w, ch_x, ch_y, ch_z,
 Lbce, vert, Rbce, tild, ____
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
        int py = y - (points[i] >> 8) + 1;
        SetSP(buf, px, py, objId, on);
        on = !on;
    }
}