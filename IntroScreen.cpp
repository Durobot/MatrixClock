#include "sys/_types.h"
#include "Arduino.h"
//#include <stdio.h>

#include <PxMatrix.h>

#include "common.h"
#include "IntroScreen.h"
#include "Application.h"

#define INTO_MILLIS 2255

extern Application app;
extern PxMATRIX display;


const struct RGB888 IntroScreen::clrs_light[] =
{
  { .r = 0xFF, .g = 0x99, .b = 0x00 },
  { .r = 0xFF, .g = 0xC6, .b = 0x00 },
  { .r = 0x34, .g = 0xB6, .b = 0x71 },
  { .r = 0x00, .g = 0x90, .b = 0x9d },
  { .r = 0xC5, .g = 0x00, .b = 0x19 },
  { .r = 0x92, .g = 0x00, .b = 0x63 },
  { .r = 0x50, .g = 0x00, .b = 0xAA }
};
const struct RGB888 IntroScreen::clrs_dark[] =
{
  { .r = 0x9b, .g = 0x5d, .b = 0x00 },
  { .r = 0xA1, .g = 0x7D, .b = 0x00 },
  { .r = 0x1D, .g = 0x6A, .b = 0x41 },
  { .r = 0x00, .g = 0x41, .b = 0x46 },
  { .r = 0x7E, .g = 0x00, .b = 0x10 },
  { .r = 0x59, .g = 0x00, .b = 0x3C },
  { .r = 0x2A, .g = 0x00, .b = 0x59 }
};


IntroScreen::IntroScreen(unsigned int scr_id) : Screen(scr_id)
{
	//this->wifiScreen = app.findScreen(SCR_WIFI);
	//Serial.print(F("IntroScreen constructor - wifiScreen = "));
  //Serial.println((long long unsigned int)this->wifiScreen, HEX);
}

IntroScreen::~IntroScreen()
{}

void IntroScreen::update(unsigned long frame_millis, unsigned long prev_frame_millis)
{
  /*
  Serial.print(F("Hello, I'm IntroScreen "));
  Serial.print(this->id);
  Serial.print(F(", frame_millis = "));
  Serial.print(frame_millis);
  Serial.print(F(", prev_frame_millis = "));
  Serial.println(prev_frame_millis);
  */

  if(this->start_millis == 0)
  {
    this->start_millis = frame_millis; // Initialize
    display.setBrightness(255); // Set the brightness of the panels (default is 255)
    return; // return early, to avoid a huge else block
  }

  unsigned long scr_life_millis = frame_millis - this->start_millis;
  if(scr_life_millis >= INTO_MILLIS)
  {
    this->start_millis = 0;
    app.switchToScreen(SCR_WIFI);
  }
  else
    if(scr_life_millis >= INTO_MILLIS - 255)
    {
      /*
      Screen* wifi_scr = app.findScreen(SCR_CLOCK);
      if(wifi_scr != NULL)
        wifi_scr->update(frame_millis, prev_frame_millis);
      */
      // Set the brightness of the panels (default is 255)
      display.setBrightness(255 - scr_life_millis);
    }
}

// static
void IntroScreen::drawLine(int16_t x, uint16_t clr)
{
  //
}