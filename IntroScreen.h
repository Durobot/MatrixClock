#ifndef _INTROSCREEN_H_
#define _INTROSCREEN_H_

#include "Screen.h"

class IntroScreen : public Screen
{
protected:
  // When this screen became active
	unsigned long start_millis = 0;
	//Screen* wifiScreen = NULL;

public:
	IntroScreen(unsigned int scr_id);
  ~IntroScreen();

	virtual void update(unsigned long frame_millis, unsigned long prev_frame_millis);
};

#endif
