#include "Arduino.h"
//#include <stdio.h>

#include "common.h"
#include "IntroScreen.h"
#include "Application.h"

extern Application app;

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
  Serial.print(F("Hello, I'm IntroScreen "));
  Serial.print(this->id);
  Serial.print(F(", frame_millis = "));
  Serial.print(frame_millis);
  Serial.print(F(", prev_frame_millis = "));
  Serial.println(prev_frame_millis);

  if(this->start_millis == 0)
    this->start_millis = frame_millis; // Initialize
  else
    if(frame_millis - this->start_millis >= 2400)
    {
      this->start_millis = 0;
      app.switchToScreen(SCR_WIFI);
    }
    else
      if(frame_millis - this->start_millis >= 2000)
      {
        Screen* wifi_scr = app.findScreen(SCR_WIFI);
        if(wifi_scr != NULL)
          wifi_scr->update(frame_millis, prev_frame_millis);
      }
}
