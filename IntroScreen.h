#ifndef _INTROSCREEN_H_
#define _INTROSCREEN_H_

#include "common.h"
#include "Screen.h"

#define LINE_COUNT 7

class IntroScreen : public Screen
{
protected:
  // When this screen became active
  unsigned long start_millis = 0;

  // Line colors, darker and lighter hues 
  static const struct RGB888 clrs_light[LINE_COUNT];
  static const struct RGB888 clrs_dark[LINE_COUNT];

  // Index of the last line currently drawn, no line has been drawn if
  // = LINE_COUNT (or greater)
  uint16_t last_line_idx = 0;
  // Current x position of the lines
  uint16_t lines_x[LINE_COUNT] = { 0, 0, 0, 0, 0, 0, 0 };
  // Line animation counter (milliseconds since the last position update)
  uint16_t line_move_counter = 0;

  // Draw slanted (similarly to / ) bi-colored line
  static void drawLine(int16_t x, uint16_t clr_idx);

public:
	IntroScreen(unsigned int scr_id);
	~IntroScreen();

	virtual void update(unsigned long frame_millis, unsigned long prev_frame_millis);
};

#endif
