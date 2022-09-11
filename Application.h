#include "stdint.h"
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

    Screen* screens[3] = { new IntroScreen(SCR_INTRO), new WifiScreen(SCR_WIFI), new ClockScreen(SCR_CLOCK) };
    Screen* active_screen = screens[0];

    // Current screen brightness. Since pxMatrix can't report current screen
    // brightness, we have to save it here.
    uint8_t cur_brightness = 0;

public:
    Application();
    ~Application();

    void update();
    Screen* findScreen(unsigned int scr_id);
    bool switchToScreen(unsigned int scr_id);

    // Set current brightness.
    // Saves set brightness, so that it can be retrieved later.
    void setBright(uint8_t bright);
    // Get current brightness
    uint8_t getBright();
};

#endif
