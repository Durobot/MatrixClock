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

  inline static void drawHourTens(uint8_t hr_tens);
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
