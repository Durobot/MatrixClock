#include <Arduino.h>
#include <soc/gpio_struct.h> // For PxMatrix
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

// -- Dimming constants --
#define DIMMING_TRANS_START_SEC (BRIGHT_END_HOUR * (60 * 60) + BRIGHT_END_MINUTE * 60 + BRIGHT_END_SECOND)
#define DIMMING_TRANS_END_SEC   (DIM_START_HOUR * (60 * 60)  + DIM_START_MINUTE * 60  + DIM_START_SECOND)

#if DIMMING_TRANS_START_SEC == DIMMING_TRANS_END_SEC
#  error "Dim period must start at least 1 second after the end of Bright period!"
#endif

// Fadeout (dimming) slope, see https://www.mathsisfun.com/equation_of_line.html
const float dimming_m = (DIMMING_TRANS_END_SEC > DIMMING_TRANS_START_SEC) ?
  (float)(DIM_BRIGHTNESS - NORMAL_BRIGHTNESS) / (float)(DIMMING_TRANS_END_SEC - DIMMING_TRANS_START_SEC) :
  (float)(DIM_BRIGHTNESS - NORMAL_BRIGHTNESS) / (float)(DIMMING_TRANS_END_SEC + (24 * 60 * 60) - DIMMING_TRANS_START_SEC);
// ----------------------

// - Brightening constants -
#define BRIGHTEN_TRANS_START_SEC (DIM_END_HOUR * (60 * 60)      + DIM_END_MINUTE * 60      + DIM_END_SECOND)
#define BRIGHTEN_TRANS_END_SEC   (BRIGHT_START_HOUR * (60 * 60) + BRIGHT_START_MINUTE * 60 + BRIGHT_START_SECOND)

#if BRIGHTEN_TRANS_START_SEC == BRIGHTEN_TRANS_END_SEC
#  error "Bright period must start at least 1 second after the end of Dim period!"
#endif

// Brightening slope, see https://www.mathsisfun.com/equation_of_line.html
const float brighten_m = (BRIGHTEN_TRANS_END_SEC > BRIGHTEN_TRANS_START_SEC) ?
  (float)(NORMAL_BRIGHTNESS - DIM_BRIGHTNESS) / (float)(BRIGHTEN_TRANS_END_SEC - BRIGHTEN_TRANS_START_SEC) :
  (float)(NORMAL_BRIGHTNESS - DIM_BRIGHTNESS) / (float)(BRIGHTEN_TRANS_END_SEC + (24 * 60 * 60) - BRIGHTEN_TRANS_START_SEC);
// ------------------------

const struct RGB888 black_clr      = { .r =   0, .g =   0, .b =   0 };
const struct RGB888 yellow1_clr    = { .r =  64, .g =  64, .b =   0 };
const struct RGB888 yellow2_clr    = { .r = 128, .g = 128, .b =   0 };
const struct RGB888 yellow3_clr    = { .r = 192, .g = 192, .b =   0 };
const struct RGB888 yellow4_clr    = { .r = 255, .g = 255, .b =   0 };
const struct RGB888 green1_clr     = { .r =   0, .g =  64, .b =   0 };
const struct RGB888 green2_clr     = { .r =   0, .g = 128, .b =   0 };
const struct RGB888 green3_clr     = { .r =   0, .g = 192, .b =   0 };
const struct RGB888 green4_clr     = { .r =   0, .g = 255, .b =   0 };
const struct RGB888 blue1_clr      = { .r =   0, .g =   0, .b =  64 };
const struct RGB888 blue2_clr      = { .r =   0, .g =   0, .b = 128 };
const struct RGB888 blue3_clr      = { .r =   0, .g =   0, .b = 192 };
const struct RGB888 blue4_clr      = { .r =   0, .g =   0, .b = 255 };
const struct RGB888 red1_clr       = { .r =  64, .g =   0, .b =   0 };
const struct RGB888 red2_clr       = { .r = 128, .g =   0, .b =   0 };
const struct RGB888 red3_clr       = { .r = 192, .g =   0, .b =   0 };
const struct RGB888 red4_clr       = { .r = 255, .g =   0, .b =   0 };
const struct RGB888 magenta1_clr   = { .r =  64, .g =   0, .b =  64 };
const struct RGB888 magenta2_clr   = { .r = 128, .g =   0, .b = 128 };
const struct RGB888 magenta3_clr   = { .r = 192, .g =   0, .b = 192 };
const struct RGB888 magenta4_clr   = { .r = 255, .g =   0, .b = 255 };
const struct RGB888 l_magenta1_clr = { .r =  64, .g =  32, .b =  64 };
const struct RGB888 l_magenta2_clr = { .r = 128, .g =  48, .b = 128 };
const struct RGB888 l_magenta3_clr = { .r = 192, .g =  64, .b = 192 };
const struct RGB888 l_magenta4_clr = { .r = 255, .g =  96, .b = 255 };
const struct RGB888 cyan1_clr      = { .r =   0, .g =  64, .b =  64 };
const struct RGB888 cyan2_clr      = { .r =   0, .g = 128, .b = 128 };
const struct RGB888 cyan3_clr      = { .r =   0, .g = 192, .b = 192 };
const struct RGB888 cyan4_clr      = { .r =   0, .g = 255, .b = 255 };
const struct RGB888 l_cyan1_clr    = { .r =  32, .g =  64, .b =  64 };
const struct RGB888 l_cyan2_clr    = { .r =  48, .g = 128, .b = 128 };
const struct RGB888 l_cyan3_clr    = { .r =  64, .g = 192, .b = 192 };
const struct RGB888 l_cyan4_clr    = { .r =  96, .g = 255, .b = 255 };
const struct RGB888 grey1_clr      = { .r =  64, .g =  64, .b =  64 };
const struct RGB888 grey2_clr      = { .r = 128, .g = 128, .b = 128 };
const struct RGB888 grey3_clr      = { .r = 192, .g = 192, .b = 192 };
const struct RGB888 grey4_clr      = { .r = 255, .g = 255, .b = 255 };
#define white_clr   grey4_clr;

const struct RGB888 special_hour_purple_clr = { .r = 171, .g =  14, .b = 199 };

/*
#define COLR_HOUR     yellow2_clr
#define COLR_MIN      green2_clr
#define COLR_CALENDAR grey1_clr
#define COLR_SEC      cyan2_clr
*/
#define COLR_HOUR     special_hour_purple_clr
#define COLR_MIN      cyan3_clr
#define COLR_CALENDAR yellow2_clr
#define COLR_SEC      grey2_clr

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
    // Set the initial brightness of the panel (max is 255)
    // for the fade in process
    this->setBright(0);
    // Calculate target brightness of the fade in
    this->fadein_target_bright = ClockScreen::calcCurBrightness();

// DEBUGGING CODE, REMOVE
//my_TZ.setTime(21, 59, 25, 1, 1, 2018);

    // Initialize hour, minutes and calendar_str
    if(this->hour == 255)
      this->hour = my_TZ.hour();

    if(this->mins == 255)
      this->mins = my_TZ.minute();

    this->regenCalendarStr();

    // Draw hour, minutes and calendar_str for the first time
    ClockScreen::drawHourTens(this->hour / 10, 0);
    ClockScreen::drawHourOnes(this->hour % 10);
    ClockScreen::drawMinutesTens(this->mins / 10);
    ClockScreen::drawMinutesOnes(this->mins % 10);
    ClockScreen::drawCalendar();

    return;
  }

  unsigned long scr_life_millis = frame_millis - this->start_millis;
  // Fade in
  if(scr_life_millis <= this->fadein_target_bright)
    this->setBright(scr_life_millis);

  uint16_t eztime_millis = ms(); // 0 - 999
  if(eztime_millis >= 1000 - (DIGIT_TRANS_MILLIS << 1)) // Second is about to change
  {
    uint8_t frame_idx = (eztime_millis >= 1000 - DIGIT_TRANS_MILLIS) ? 1 : 0;
    if(!this->sec_trans_frame_shown[frame_idx]) // Show transition frames once
    {
      uint8_t ezt_sec = my_TZ.second();
      uint8_t sec_ones = ezt_sec % 10;

      if(sec_ones == 9)
      {
        uint8_t sec_tens = ezt_sec / 10;

        // 1 is transition index. If sec_tens == 5, it indicates that we want 5->0 transition, not 5->6.
        // If sec_tens != 5, transition index is ignored.
        GlyphRenderer::drawSmallDigitTrans(sec_tens, 1, frame_idx, SCREEN_WIDTH - 2 * SMALL_CHAR_WIDTH, BIG_DIGIT_HEIGHT + 1, COLR_SEC);

        if(sec_tens == 5) // 60th second ('59' is currently displayed) is about to end
        {
          uint8_t ezt_mins = my_TZ.minute();
          uint8_t min_ones = ezt_mins % 10;

          // 2nd argument == 0 is transition index. If min_ones == 5, it indicates that we want 5->6 transition, not 5->0.
          // If min_ones != 5, transition index is ignored.
          GlyphRenderer::drawBigDigitTrans(min_ones, 0, frame_idx, 3 * BIG_DIGIT_WIDTH + 1, 0, COLR_MIN);

          if(min_ones == 9) // Next ten minutes is about to start
          {
            uint8_t min_tens = ezt_mins / 10;
            // 2nd argument == 1 is transition index. If min_tens == 5, it indicates that we want 5->0 transition, not 5->6.
            // If min_tens != 5, transition index is ignored.
            GlyphRenderer::drawBigDigitTrans(min_tens, 1, frame_idx, 2 * BIG_DIGIT_WIDTH + 1, 0, COLR_MIN);

            if(min_tens == 5) // Next hour is about to start
            {
              uint8_t ezt_hr = my_TZ.hour();
              uint8_t hr_ones = ezt_hr % 10;

              // 2nd argument == 0 is transition index.
              // If hr_ones == 5, it indicates that we want 5->6 transition, not 5->0.
              // If hr_ones is not 3 or 5, transition index is ignored.
              GlyphRenderer::drawBigDigitTrans(hr_ones, 0, frame_idx, BIG_DIGIT_WIDTH - 1, 0, COLR_HOUR);

              uint8_t hr_tens = ezt_hr / 10;
              if(hr_ones == 9)
              {
                if(hr_tens == 0) // 9 hours -> 10 hours
                  ClockScreen::drawHourTens(1, (frame_idx + 1) * BIG_DIGIT_HEIGHT / 3); // Empty space -> 1 animation
                else // 19 hours -> 20 hours
                  // 2nd argument == 0 is transition index. Since hr_tens is not 2 or 5 it is ignored.
                  GlyphRenderer::drawBigDigitTrans(hr_tens, 0, frame_idx, BIG_DIGIT_WIDTH - 1, 0, COLR_HOUR);
              }
              else
                if(hr_tens == 2 && hr_ones == 3) // 2 -> Empty space animation
                  GlyphRenderer::paintRect(0, // x
                                           0, // y
                                           BIG_DIGIT_WIDTH - 1, // width
                                           (frame_idx + 1) * BIG_DIGIT_HEIGHT / 3, // height
                                           black_clr); // color
            } // if(min_tens == 5)
          } // if(min_ones == 9)
        } // if(sec_tens == 5)
      } // if(sec_ones == 9)

      // 0 is transition index. If sec_ones == 5, it indicates that we want 5->6 transition, not 5->0.
      // If sec_tens != 5, transition index is ignored.
      GlyphRenderer::drawSmallDigitTrans(sec_ones, 0, frame_idx, SCREEN_WIDTH - SMALL_CHAR_WIDTH, BIG_DIGIT_HEIGHT + 1, COLR_SEC);
      this->sec_trans_frame_shown[frame_idx] = true;
    }
    return;
  }

  if(this->sec_trans_frame_shown[0]) // Second has just changed
  {
    // Reset seconds transition frame flags
    this->sec_trans_frame_shown[0] =
      this->sec_trans_frame_shown[1] = false;

    // Set brightness according to normal / dim brighness cycle.
    // Since we only take seconds into account when calculating this
    // brightness, calling calcCurBrightness() more than once a second
    // would be a waste.
    this->setBright(ClockScreen::calcCurBrightness());

    // Draw seconds
    uint8_t ezt_sec = my_TZ.second();
    GlyphRenderer::drawSmallChar(ezt_sec / 10 + '0', SCREEN_WIDTH - 2 * SMALL_CHAR_WIDTH, BIG_DIGIT_HEIGHT + 1, COLR_SEC);
    GlyphRenderer::drawSmallChar(ezt_sec % 10 + '0', SCREEN_WIDTH - SMALL_CHAR_WIDTH,     BIG_DIGIT_HEIGHT + 1, COLR_SEC);

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
        ClockScreen::drawHourTens(hr_tens, 0);
        // New day has just started
        if(ezt_hr == 0)
        {
          this->regenCalendarStr();
          ClockScreen::drawCalendar();
        }
      }
      this->hour = ezt_hr;
    }
  } // if(this->sec_trans_frame_shown[0])
}

// static
uint8_t ClockScreen::calcCurBrightness()
{
  uint8_t hr = my_TZ.hour();
  uint8_t min = my_TZ.minute();
  uint8_t sec = my_TZ.second();
  uint32_t day_sec = hr * (60 * 60) + min * 60 + sec;

  // Normal brightness period
  if(ClockScreen::isSecondBetween(day_sec, BRIGHTEN_TRANS_END_SEC, DIMMING_TRANS_START_SEC))
    return NORMAL_BRIGHTNESS;

  // Dim brightness period
  if(ClockScreen::isSecondBetween(day_sec, DIMMING_TRANS_END_SEC, BRIGHTEN_TRANS_START_SEC))
    return DIM_BRIGHTNESS;

  // See two-point form of straight line equation in
  // https://www.vedantu.com/maths/equation-of-a-straight-line
  if(ClockScreen::isSecondBetween(day_sec, DIMMING_TRANS_START_SEC, DIMMING_TRANS_END_SEC)) // In dimming transition?
    if(DIMMING_TRANS_START_SEC > DIMMING_TRANS_END_SEC && // Dimming period crossing midnight
       day_sec < DIMMING_TRANS_START_SEC) // and we're currently in the next day
      return dimming_m * (day_sec + (60 * 60 * 24) - DIMMING_TRANS_START_SEC) + NORMAL_BRIGHTNESS + 0.5;
    else
      return dimming_m * (day_sec - DIMMING_TRANS_START_SEC) + NORMAL_BRIGHTNESS + 0.5;

  if(ClockScreen::isSecondBetween(day_sec, BRIGHTEN_TRANS_START_SEC, BRIGHTEN_TRANS_END_SEC)) // In brightening transition?
    if(BRIGHTEN_TRANS_START_SEC > BRIGHTEN_TRANS_END_SEC && // Brightening period crossing midnight
       day_sec < BRIGHTEN_TRANS_START_SEC) // and we're currently in the next day
      return brighten_m * (day_sec + (60 * 60 * 24) - BRIGHTEN_TRANS_START_SEC) + DIM_BRIGHTNESS + 0.5;
    else
      return brighten_m * (day_sec - BRIGHTEN_TRANS_START_SEC) + DIM_BRIGHTNESS + 0.5;

  // Should not get here
  return 255;
}

// static
bool ClockScreen::isSecondBetween(uint32_t sec, uint32_t day_sec_1, uint32_t day_sec_2)
{
  if(day_sec_1 <= day_sec_2) // [...day_sec_1---day_sec_2...]
  {
    if(sec >= day_sec_1 && sec <= day_sec_2)
      return true;
  }
  else // [---day_sec_2...day_sec_1---]
    if(sec <= day_sec_2 || sec >= day_sec_1)
      return true;

  return false;
}

void ClockScreen::setBright(uint8_t bright)
{
  uint8_t cur_bright = app.getBright();
  if(bright == cur_bright) // No change? (unlikely)
    return;

  app.setBright(bright);

  // Are we crossing the threshold?
  if((cur_bright <= LARGE_DIGIT_GRADIENT_THRESHOLD && bright > LARGE_DIGIT_GRADIENT_THRESHOLD) ||
     (cur_bright > LARGE_DIGIT_GRADIENT_THRESHOLD && bright <= LARGE_DIGIT_GRADIENT_THRESHOLD))
  {
    // Redraw hour and minutes, since 
    ClockScreen::drawHourTens(this->hour / 10, 0);
    ClockScreen::drawHourOnes(this->hour % 10);
    ClockScreen::drawMinutesTens(this->mins / 10);
    ClockScreen::drawMinutesOnes(this->mins % 10);
  }
}

inline void ClockScreen::regenCalendarStr()
{
  strncpy(this->calendar_str, my_TZ.dateTime("d-m D").c_str(), sizeof this->calendar_str - 1);
}

inline void ClockScreen::drawHourTens(uint8_t hr_tens, uint16_t lines_limit)
{
  if(hr_tens == 0) // Paint over the previous tens of hours
    GlyphRenderer::paintRect(0, // x
                             0, // y
                             BIG_DIGIT_WIDTH - 1, // width
                             BIG_DIGIT_HEIGHT - 1, // height
                             black_clr); // color
  else
    GlyphRenderer::drawBigDigit(hr_tens, -1, 0, lines_limit, COLR_HOUR); // x == -1 to move the digit to the left, it's OK
}

inline void ClockScreen::drawHourOnes(uint8_t hr_ones)
{
  GlyphRenderer::drawBigDigit(hr_ones, BIG_DIGIT_WIDTH - 1, 0, 0, COLR_HOUR);
}

inline void ClockScreen::drawMinutesTens(uint8_t min_tens)
{
  GlyphRenderer::drawBigDigit(min_tens, 2 * BIG_DIGIT_WIDTH + 1, 0, 0, COLR_MIN);
}

inline void ClockScreen::drawMinutesOnes(uint8_t min_ones)
{
  GlyphRenderer::drawBigDigit(min_ones, 3 * BIG_DIGIT_WIDTH + 1, 0, 0, COLR_MIN);
}

inline void ClockScreen::drawCalendar()
{
  GlyphRenderer::drawSmallString(this->calendar_str, 0, BIG_DIGIT_HEIGHT + 1, COLR_CALENDAR);
}
