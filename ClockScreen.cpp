#include "stdint.h"
//#include "WiFiType.h"
#include <Arduino.h>
#include <PxMatrix.h>
#include <ezTime.h>

#include "common.h"
#include "ClockScreen.h"
#include "Application.h"
#include "GlyphRenderer.h"

extern Application app;
extern PxMATRIX display;
extern Timezone my_TZ;

const struct RGB888 yellow_clr = { .r = 128, .g = 128, .b = 0 };

ClockScreen::ClockScreen(unsigned int scr_id) : Screen(scr_id)
{}

ClockScreen::~ClockScreen()
{}

void ClockScreen::update(unsigned long frame_millis, unsigned long prev_frame_millis)
{
  if(this->start_millis == 0)
  {
    this->start_millis = frame_millis; // Initialize
    display.clearDisplay();
    display.setBrightness(0); // Set the brightness of the panel (max is 255)
    return;
  }

  unsigned long scr_life_millis = frame_millis - this->start_millis;
  // Fade in
  if(scr_life_millis <= 255)
    display.setBrightness(scr_life_millis);

  if(secondChanged()) // ezTime function
  {
    GlyphRenderer::drawSmallString(my_TZ.dateTime("G:i:s").c_str(), 8, 12, yellow_clr);
  }

  //display.drawPixelRGB888(random(SCREEN_WIDTH), random(SCREEN_HEIGHT), random(256), random(256), random(256));
}
