#ifndef _CLOCKSCREEN_H_
#define _CLOCKSCREEN_H_

#include "Screen.h"

class ClockScreen : public Screen
{
private:
  // When this screen became active
  unsigned long start_millis = 0;

public:
	ClockScreen(unsigned int scr_id);
	virtual ~ClockScreen();

	virtual void update(unsigned long frame_millis, unsigned long prev_frame_millis);
};

#endif
