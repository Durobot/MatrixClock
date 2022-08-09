#ifndef _INTROSCREEN_H_
#define _INTROSCREEN_H_

#include "common.h"
#include "Screen.h"

class IntroScreen : public Screen
{
protected:
  // When this screen became active
	unsigned long start_millis = 0;
	//Screen* wifiScreen = NULL;

  static const struct RGB888 clrs_light[];
  static const struct RGB888 clrs_dark[];

  // Draw slanted (similarly to / ) bi-colored line
  static void drawLine(int16_t x, uint16_t clr);

public:
	IntroScreen(unsigned int scr_id);
	~IntroScreen();

	virtual void update(unsigned long frame_millis, unsigned long prev_frame_millis);
};

#endif
