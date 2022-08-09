#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h> // uint8_t

#define SCREEN_WIDTH  64
#define SCREEN_HEIGHT 32

#define SCR_INTRO 1u
#define SCR_WIFI  2u
#define SCR_CLOCK 3u

struct RGB888
{
  uint8_t r, g, b;
};

#endif
