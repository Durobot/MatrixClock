#ifndef _GLYPHRENDERER_H_
#define _GLYPHRENDERER_H_

#include <stdint.h> // uint8_t, uint16_t

#define BIG_DIGIT_WIDTH   16
#define BIG_DIGIT_HEIGHT  24
#define SMALL_CHAR_WIDTH   6
#define SMALL_CHAR_HEIGHT  8

class GlyphRenderer
{
public:
    GlyphRenderer();
    ~GlyphRenderer();

    static void drawBigDigit(uint8_t digit, uint16_t x, uint16_t y);
    static void drawBigTransDigit(uint8_t digit, uint8_t trans_idx, uint16_t x, uint16_t y);
    static void drawSmallChar(char c, uint16_t x, uint16_t y);
    static void drawSmallTransDigit(uint8_t digit, uint8_t trans_idx, uint16_t x, uint16_t y);
};

#endif
