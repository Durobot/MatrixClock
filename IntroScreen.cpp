#include <Arduino.h>

#include <soc/gpio_struct.h> // For PxMatrix
#include <PxMatrix.h>

#include "common.h"
#include "config.h"
#include "IntroScreen.h"
#include "Application.h"

// Fadeout slope, see https://www.mathsisfun.com/equation_of_line.html
const float intro_fadeout_m = -(float)INTRO_WIFI_BRIGHTNESS / (float)INTO_FADEOUT_MILLIS;

extern Application app;
extern PxMATRIX display;

const struct RGB888 IntroScreen::clrs_light[] =
{
  //{ .r = 0xFF, .g = 0x99, .b = 0x00 },
  { .r = 0xFF, .g = 0x60, .b = 0x00 }, // Orange
  //{ .r = 0xFF, .g = 0xC6, .b = 0x00 },
  { .r = 0xFF, .g = 0xFF, .b = 0x00 }, // Yellow
  //{ .r = 0x34, .g = 0xB6, .b = 0x71 },
  { .r = 0x00, .g = 0xFF, .b = 0x00 }, // Green
  //{ .r = 0x00, .g = 0x90, .b = 0x9d },
  { .r = 0x00, .g = 0xAA, .b = 0xAA }, // Cyan
  //{ .r = 0xC5, .g = 0x00, .b = 0x19 },
  { .r = 0xFF, .g = 0x00, .b = 0x00 }, // Red
  //{ .r = 0x92, .g = 0x00, .b = 0x63 },
  { .r = 0xFF, .g = 0x00, .b = 0x60 }, // Wine red (ish)
  //{ .r = 0x50, .g = 0x00, .b = 0xAA }
  { .r = 0x60, .g = 0x00, .b = 0xFF }  // Indigo
};
/*
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
*/

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
  if(this->start_millis == 0)
  {
    this->start_millis = frame_millis; // Initialize
    display.clearDisplay();
    display.setBrightness(INTRO_WIFI_BRIGHTNESS); // Set the brightness of the panel (max is 255)
  }
  else
  {
    unsigned long scr_life_millis = frame_millis - this->start_millis;
    if(scr_life_millis >= INTO_SCR_MILLIS)
    {
      this->resetScreen();
      app.switchToScreen(SCR_WIFI);
      return;
    }

    // Fade to black before we switch to the clock screen.
    // see https://www.mathsisfun.com/equation_of_line.html
    if(scr_life_millis >= INTO_SCR_MILLIS - INTO_FADEOUT_MILLIS)
      display.setBrightness(intro_fadeout_m * (scr_life_millis - (INTO_SCR_MILLIS - INTO_FADEOUT_MILLIS)) + INTRO_WIFI_BRIGHTNESS);
  }

  // -- Draw the lines --
  //
  if(this->last_line_idx >= LINE_COUNT)
    return;

  this->line_move_counter += (frame_millis - prev_frame_millis);
  if(this->line_move_counter < 8) // If we divide by 8, do we get 0?
    return; // If yes, we haven't got enough movement for 1 pixel yet.

  // Invcement curren line's X position 1 pixel every 8 milliseconds
  this->lines_x[this->last_line_idx] += this->line_move_counter >> 3;
  this->line_move_counter = 0;

  display.clearDisplay();
  for(uint16_t i = 0; i <= this->last_line_idx; i++)
    IntroScreen::drawLine(this->lines_x[i], i);

  if(this->lines_x[this->last_line_idx] >= SCREEN_WIDTH - (this->last_line_idx << 2))
    this->last_line_idx++;
}

// Draw slanted (similarly to / ) bi-colored line
// static
void IntroScreen::drawLine(int16_t x, uint16_t clr_idx)
{
  const struct RGB888 light = IntroScreen::clrs_light[clr_idx];
  //const struct RGB888 dark  = IntroScreen::clrs_dark[clr_idx];

  for(uint16_t y = 0; y < SCREEN_HEIGHT; y++)
  {
    if(x < 0)
      return;

    // drawPixelRGB888 checks that x and y are > 0 - no need to do it here
    display.drawPixelRGB888(x - 2, y, light.r >> 2, light.g >> 2, light.b >> 2);
    display.drawPixelRGB888(x - 1, y, light.r, light.g, light.b);
    display.drawPixelRGB888(x,     y, light.r, light.g, light.b);
    if(y & 0x01)
      x--;
  }
}

// Re-initialize all fields, preparing the screen to go (again)
void IntroScreen::resetScreen()
{
  this->start_millis = 0;
  this->line_move_counter =
    this->last_line_idx = 0;
  for(int i = 0; i < LINE_COUNT; i++)
    this->lines_x[i] = 0;
}
