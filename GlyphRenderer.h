#ifndef _GLYPHRENDERER_H_
#define _GLYPHRENDERER_H_

#include <stdint.h> // uint8_t, uint16_t

#include "common.h"

#define BIG_DIGIT_WIDTH   16
#define BIG_DIGIT_HEIGHT  24
#define SMALL_CHAR_WIDTH   6
#define SMALL_CHAR_HEIGHT  8

class GlyphRenderer
{
public:
    GlyphRenderer();
    ~GlyphRenderer();

    // Draw large (BIG_DIGIT_WIDTH x BIG_DIGIT_HEIGHT) digit.
    // lines_limit sets the limit of horizontal scanlines to draw, counting from the top.
    // If lines_limit == 0, this limit is ignored, and all BIG_DIGIT_HEIGHT lines are rendered.
    static void drawBigDigit(uint8_t digit, int16_t x, int16_t y, uint16_t lines_limit, struct RGB888 clr);
    // Draw a frame of transition (frame_idx is frame index) between two big digits.
    // trans_idx is transition index. Currently, only 5 has two transitions -
    // transition 0 is 5->6 and transition 1 is 5->9.
    // For all the other digits, trans_idx is ignored.
    static void drawBigDigitTrans(uint8_t digit, uint8_t trans_idx, uint8_t frame_idx, int16_t x, int16_t y, struct RGB888 clr);
    static void drawSmallChar(char c, uint16_t x, uint16_t y, struct RGB888 clr);
    // Draw a frame of transition (frame_idx is frame index) between two small digits.
    // trans_idx is transition index. Currently, only 5 has two transitions -
    // transition 0 is 5->6 and transition 1 is 5->9.
    // For all the other digits, trans_idx is ignored.
    static void drawSmallDigitTrans(uint8_t digit, uint8_t trans_idx, uint8_t frame_idx, uint16_t x, uint16_t y, struct RGB888 clr);
    static void drawSmallString(const char* str, uint16_t x, uint16_t y, struct RGB888 clr);
    // Paint a rectangle with color clr
    static void paintRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, struct RGB888 clr);
};

#endif
