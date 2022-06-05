#include "Arduino.h"
//#include <stdio.h>

#include "WifiScreen.h"

WifiScreen::WifiScreen(unsigned int scr_id) : Screen(scr_id)
{}

WifiScreen::~WifiScreen()
{}

void WifiScreen::update(unsigned long frame_millis, unsigned long prev_frame_millis)
{
  Serial.print(F("Hello, I'm WifiScreen "));
  Serial.print(this->id);
  Serial.print(F(", frame_millis = "));
  Serial.print(frame_millis);
  Serial.print(F(", prev_frame_millis = "));
  Serial.println(prev_frame_millis);
}
