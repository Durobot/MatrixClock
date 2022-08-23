#ifndef _WIFISCREEN_H_
#define _WIFISCREEN_H_

#include "Screen.h"

class WifiScreen : public Screen
{
private:
  // When this screen became active
  unsigned long start_millis = 0;
  // End of our attempt to connect to wifi network
  unsigned long wifi_result_millis = 0;

public:
	WifiScreen(unsigned int scr_id);
	virtual ~WifiScreen();

	virtual void update(unsigned long frame_millis, unsigned long prev_frame_millis);
};

#endif
