#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "common.h"
#include "Screen.h"
#include "IntroScreen.h"
#include "WifiScreen.h"
#include "ClockScreen.h"

class Application
{
private:
    unsigned long frame_millis = 0;
    unsigned long prev_frame_millis = 0;

    Screen* screens[2] = { new IntroScreen(SCR_INTRO), new ClockScreen(SCR_CLOCK) };
    Screen* active_screen = screens[0];

public:
    Application();
    ~Application();

    void update();
    Screen* findScreen(unsigned int scr_id);
    bool switchToScreen(unsigned int scr_id);
};

#endif
