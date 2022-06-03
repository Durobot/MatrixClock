#include <stdio.h>

#include "common.h"
#include "IntroScreen.h"
#include "Application.h"

extern Application app;

IntroScreen::IntroScreen(unsigned int scr_id) : Screen(scr_id)
{
	//this->wifiScreen = app.findScreen(SCR_WIFI);
	printf("IntroScreen constructor - wifiScreen = %x", this->wifiScreen);
}

void IntroScreen::update()
{
	printf("Hello, I'm IntroScreen %u\n", this->id);
	this->counter--;
	if(this->counter == 0)
		app.switchToScreen(SCR_WIFI);
	else
	{
		Screen* wifiScr = app.findScreen(SCR_WIFI);
		if(wifiScr != NULL && this->counter < 30)
			wifiScr->update();
	}		
}

