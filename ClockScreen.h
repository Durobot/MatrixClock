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

  // Target brightness for the initial fadein.
  // Calculated once on initialization.
  uint8_t fadein_target_bright = 0;

  // Draw tens of hours in ther standard position and color.
  // lines_limit sets the limit of horizontal scanlines to draw, counting from the top.
  // This parameter is used for emtpy space -> 1 animation, when 9 hours becomes 10.
  // If lines_limit == 0, this limit is ignored, and all BIG_DIGIT_HEIGHT lines are rendered.
  inline static void drawHourTens(uint8_t hr_tens, uint16_t lines_limit);
  inline static void drawHourOnes(uint8_t hr_ones);
  inline static void drawMinutesTens(uint8_t min_tens);
  inline static void drawMinutesOnes(uint8_t min_ones);
  inline void drawCalendar();

  // Calculate current screen brightness, based on the screen brightness cycle.
  // Change constants in config.h and recompile to change settings.
  static uint8_t calcCurBrightness();
  // Returns true if second sec is within time range defined by day_sec_1 and day_sec_2.
  // If day_sec_2 is greater than or equal to day_sec_1, sec must be between day_sec_1 and day_sec_2:
  //   [...day_sec_1---day_sec_2...]
  // If day_sec_2 is less than day_sec_1, sec must be less than or equal to day_sec_2 or
  // greater than or equal to day_sec_1:
  //   [---day_sec_2...day_sec_1---]
  static bool isSecondBetween(uint32_t sec, uint32_t day_sec_1, uint32_t day_sec_2);

  // Re-generate calendar string. Done at initialization and after midnight.
  inline void regenCalendarStr();

  // Set brightness screen. Does several things in addition to that -
  // Stores current brightness in a field, since pxMatrix has no getter function for it;
  // Re-draws hours and minutes if gradient / no gradient threshold ( LARGE_DIGIT_GRADIENT_THRESHOLD )
  // is crossed by this change of brightness.
  void setBright(uint8_t bright);

public:
	ClockScreen(unsigned int scr_id);
	virtual ~ClockScreen();

	virtual void update(unsigned long frame_millis, unsigned long prev_frame_millis);
};

#endif
