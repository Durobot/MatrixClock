#ifndef _WIFISCREEN_H_
#define _WIFISCREEN_H_

#include "Screen.h"

class WifiScreen : public Screen
{
protected:
	int bar;

public:
	WifiScreen(unsigned int scr_id);
  virtual ~WifiScreen();

	virtual void update(unsigned long frame_millis, unsigned long prev_frame_millis);
};

#endif
