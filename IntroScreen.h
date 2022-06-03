#ifndef _INTROSCREEN_H_
#define _INTROSCREEN_H_

#include "Screen.h"

class IntroScreen : public Screen
{
protected:
	int counter = 100;
	Screen* wifiScreen = NULL;

public:
	IntroScreen(unsigned int scr_id);
	virtual void update();
};

#endif

