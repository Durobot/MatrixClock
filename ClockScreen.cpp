#include "stdint.h"
#include <Arduino.h>
#include <PxMatrix.h>
#include <ezTime.h>

#include "common.h"
#include "config.h"
#include "ClockScreen.h"
#include "Application.h"
#include "GlyphRenderer.h"

extern Application app;
extern PxMATRIX display;
extern Timezone my_TZ;

const struct RGB888 yellow_clr  = { .r = 128, .g = 128, .b =   0 };
const struct RGB888 green_clr   = { .r =   0, .g = 128, .b =   0 };
const struct RGB888 magenta_clr = { .r = 128, .g =   0, .b = 128 };
const struct RGB888 cyan_clr    = { .r =   0, .g = 128, .b = 128 };
const struct RGB888 grey_clr    = { .r =  64, .g =  64, .b =  64 };

ClockScreen::ClockScreen(unsigned int scr_id) : Screen(scr_id)
{
  this->calendar_str[0] = '\0';
}

ClockScreen::~ClockScreen()
{}

void ClockScreen::update(unsigned long frame_millis, unsigned long prev_frame_millis)
{
  if(this->start_millis == 0) // Initialize
  {
    this->start_millis = frame_millis;
    display.clearDisplay();
    display.setBrightness(0); // Set the brightness of the panel (max is 255)

    // Initialize hour, minutes and calendar_str
    if(this->hour == 255)
      this->hour = my_TZ.hour();

    if(this->mins == 255)
      this->mins = my_TZ.minute();

    if(this->calendar_str[0] == '\0')
      strncpy(this->calendar_str, my_TZ.dateTime("d-m-Y j").c_str(), sizeof this->calendar_str - 1);
      //strncpy(this->wkday, my_TZ.dateTime("M j,D").c_str(), sizeof this->wkday - 1);
      //strncpy(this->wkday, my_TZ.dateTime("l").c_str(), sizeof this->wkday - 1);

    // Draw hour, minutes and calendar_str for the first time
    ClockScreen::drawHourTens(this->hour / 10);
    ClockScreen::drawHourOnes(this->hour % 10);
    ClockScreen::drawMinutesTens(this->mins / 10);
    ClockScreen::drawMinutesOnes(this->mins % 10);
    ClockScreen::drawCalendar();

    return;
  }

  unsigned long scr_life_millis = frame_millis - this->start_millis;
  // Fade in
  if(scr_life_millis <= 255)
    display.setBrightness(scr_life_millis);

  uint16_t eztime_millis = ms(); // 0 - 999
  if(eztime_millis >= 1000 - (DIGIT_TRANS_MILLIS << 1)) // Second is about to change
  {
    uint8_t frame_idx = (eztime_millis >= 1000 - DIGIT_TRANS_MILLIS) ? 1 : 0;
    if(!this->sec_trans_frame_shown[frame_idx]) // Show transition frames once
    {
      uint8_t sec = my_TZ.second();
      uint8_t sec_ones = sec % 10;
      uint8_t sec_tens = sec / 10;
      if(sec_ones == 9)
      {
        // 1 is transition index. If sec_tens == 5, it indicates that we want 5->0 transition, not 5->6.
        // If sec_tens != 5, transition index is ignored.
        GlyphRenderer::drawSmallDigitTrans(sec_tens, 1, frame_idx, SCREEN_WIDTH - 2 * SMALL_CHAR_WIDTH, BIG_DIGIT_HEIGHT, cyan_clr);

        if(sec_tens == 5) // 60th second ('59' is currently displayed) is about to end
        {
          uint8_t mins = my_TZ.minute();
          uint8_t min_ones = mins % 10;
          uint8_t min_tens = mins / 10;
          GlyphRenderer::drawBigDigitTrans(min_ones, frame_idx, 48, 0, green_clr);

          if(min_ones == 9) // Next ten minutes is about to start
            GlyphRenderer::drawBigDigit(min_tens, 32, 0, green_clr);
        }
      }
      // 0 is transition index. If sec_ones == 5, it indicates that we want 5->6 transition, not 5->0.
      // If sec_tens != 5, transition index is ignored.
      GlyphRenderer::drawSmallDigitTrans(sec_ones, 0, frame_idx, SCREEN_WIDTH - SMALL_CHAR_WIDTH, BIG_DIGIT_HEIGHT, cyan_clr);
      this->sec_trans_frame_shown[frame_idx] = true;
    }
    return;
  }

  if(this->sec_trans_frame_shown[0]) // Second has just changed
  {
    // Reset seconds transition frame flags
    this->sec_trans_frame_shown[0] =
      this->sec_trans_frame_shown[1] = false;

    // Draw seconds
    uint8_t ezt_sec = my_TZ.second();
    GlyphRenderer::drawSmallChar(ezt_sec / 10 + '0', SCREEN_WIDTH - 2 * SMALL_CHAR_WIDTH, BIG_DIGIT_HEIGHT, cyan_clr);
    GlyphRenderer::drawSmallChar(ezt_sec % 10 + '0', SCREEN_WIDTH - SMALL_CHAR_WIDTH,     BIG_DIGIT_HEIGHT, cyan_clr);

    uint8_t ezt_mins = my_TZ.minute();
    // Minute has just changed either because seconds counter is 0,
    // or because of NTP sync, performed by ezTime in events() call.
    // See loop() in MatrixClock.ino
    if(ezt_sec == 0 || ezt_mins != this->mins)
    {
      uint8_t min_ones = ezt_mins % 10;
      ClockScreen::drawMinutesOnes(min_ones);
      // Next ten minutes just started or NYP sync caused minutes to jump
      if(min_ones == 0 || ezt_mins != this->mins)
      {
        uint8_t min_tens = ezt_mins / 10;
        ClockScreen::drawMinutesTens(min_tens);
      }
      this->mins = ezt_mins;
    }

    uint8_t ezt_hr = my_TZ.hour();
    // Hour has just changed either because minutes counter is 0,
    // or because of NTP sync, performed by ezTime in events() call.
    // See loop() in MatrixClock.ino
    if(ezt_mins == 0 || ezt_hr != this->hour)
    {
      uint8_t hr_ones = ezt_hr % 10;
      ClockScreen::drawHourOnes(hr_ones);
      // Next ten hours just started or NYP sync caused hours to jump
      if(hr_ones == 0 || ezt_hr != this->hour)
      {
        uint8_t hr_tens = ezt_hr / 10;
        ClockScreen::drawHourTens(hr_tens);
      }
      this->hour = ezt_hr;
    }
  } // if(this->sec_trans_frame_shown[0])
}

/*
void ClockScreen::drawHrMinDay()
{
    // Hours
    uint8_t hr = my_TZ.hour();
    if(hr > 9)
      GlyphRenderer::drawBigDigit(hr / 10, 0, 0, yellow_clr);
    //
    GlyphRenderer::drawBigDigit(hr % 10, 16, 0, yellow_clr);

    // Minutes
    uint8_t min = my_TZ.minute();
    GlyphRenderer::drawBigDigit(min / 10, 32, 0, green_clr);
    GlyphRenderer::drawBigDigit(min % 10, 48, 0, green_clr);

    // calendar_str
    GlyphRenderer::drawSmallString(my_TZ.dateTime("l").c_str(), 0, 24, grey_clr);
}

void ClockScreen::resetScreen()
{
  this->hour =
    this->mins = 255;

  this->calendar_str[0] = '\0';
}
*/

inline void ClockScreen::drawHourTens(uint8_t hr_tens)
{
  if(hr_tens == 0) // Paint over the previous tens of hours
    display.drawRect(0, 0, BIG_DIGIT_WIDTH - 1, BIG_DIGIT_HEIGHT, 0);
  else
    GlyphRenderer::drawBigDigit(hr_tens, -1, 0, yellow_clr); // x == -1 to move the digit to the left, it's OK
}

inline void ClockScreen::drawHourOnes(uint8_t hr_ones)
{
  GlyphRenderer::drawBigDigit(hr_ones, BIG_DIGIT_WIDTH - 1, 0, yellow_clr);
}

inline void ClockScreen::drawMinutesTens(uint8_t min_tens)
{
  GlyphRenderer::drawBigDigit(min_tens, 2 * BIG_DIGIT_WIDTH + 1, 0, green_clr);
}

inline void ClockScreen::drawMinutesOnes(uint8_t min_ones)
{
  GlyphRenderer::drawBigDigit(min_ones, 3 * BIG_DIGIT_WIDTH + 1, 0, green_clr);
}

inline void ClockScreen::drawCalendar()
{
  GlyphRenderer::drawSmallString(this->calendar_str, 0, 24, grey_clr);
}
