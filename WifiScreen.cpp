#include <stdio.h>

#include "WifiScreen.h"

WifiScreen::WifiScreen(unsigned int scr_id) : Screen(scr_id)
{}

void WifiScreen::update()
{
	printf("Hello, I'm WifiScreen %u\n", this->id);
}

