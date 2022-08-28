#ifndef _CLOCKSCREEN_H_
#define _CLOCKSCREEN_H_

#include "Screen.h"

class ClockScreen : public Screen
{
private:
  // When this screen became active
  unsigned long start_millis = 0;

  // 255 means we don't know
  uint8_t hour = 255;
  uint8_t mins = 255;
  char calendar_str[20];

  // Flags indicating whether we have shown
  // this transition frame for this seconds change
  bool sec_trans_frame_shown[2] = { false, false };

  //static void drawHrMinDay();

  // Draw tens of hours in ther standard position and color.
  // lines_limit sets the limit of horizontal scanlines to draw, counting from the top.
  // This parameter is used for emtpy space -> 1 animation, when 9 hours becomes 10.
  // If lines_limit == 0, this limit is ignored, and all BIG_DIGIT_HEIGHT lines are rendered.
  inline static void drawHourTens(uint8_t hr_tens, uint16_t lines_limit);
  inline static void drawHourOnes(uint8_t hr_ones);
  inline static void drawMinutesTens(uint8_t min_tens);
  inline static void drawMinutesOnes(uint8_t min_ones);
  inline void drawCalendar();

  // Re-initialize all fields, preparing the screen to go (again)
  //void resetScreen();

public:
	ClockScreen(unsigned int scr_id);
	virtual ~ClockScreen();

	virtual void update(unsigned long frame_millis, unsigned long prev_frame_millis);
};

#endif
