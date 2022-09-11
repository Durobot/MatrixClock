#include <stdio.h>  // printf
#include <stdint.h> // uint8_t, uint16_t, uint32_t

// Screen brightness values
#define DIM_BRIGHTNESS 48
#define NORMAL_BRIGHTNESS 255

// IMPORTANT: 1) periods can't overlap.
//            2) there must be at least 1 second between the end of one
//               period and the start of the next.

// Fully dim (after transition) period start
#define DIM_START_HOUR   0
#define DIM_START_MINUTE 30
#define DIM_START_SECOND 0

// End of fully dim period, when the screen starts
// to transition from dim to normal brightness
#define DIM_END_HOUR   5
#define DIM_END_MINUTE 0
#define DIM_END_SECOND 0

// Fully bright (after transition) period start
#define BRIGHT_START_HOUR   6
#define BRIGHT_START_MINUTE 0
#define BRIGHT_START_SECOND 0

// End of fully bright period, when the screen starts
// to transition from normal to dim brightness
#define BRIGHT_END_HOUR   23
#define BRIGHT_END_MINUTE 30
#define BRIGHT_END_SECOND 0

// ----------------------------------------

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

class TzSimulator
{
private:
    uint8_t hr = 0;
    uint8_t min = 0;
    uint8_t sec = 0;

public:
    uint8_t hour()
    { return this->hr; }

    uint8_t minute()
    { return this->min; }

    uint8_t second()
    { return this->sec; }

    void set_hour(uint8_t hr)
    { this->hr = hr; }

    void set_minute(uint8_t min)
    { this->min = min; }

    void set_second(uint8_t sec)
    { this->sec = sec; }

    void nextSecond()
    {
        if(++(this->sec) >= 60)
        {
            this->sec = 0;
            if(++(this->min) >= 60)
            {
                this->min = 0;
                if(++(this->hr) >= 24)
                    this->hr = 0;
            }
        }
    }
};

TzSimulator my_TZ;

// ------------------------

bool isSecondBetween(uint32_t sec, uint32_t day_sec_1, uint32_t day_sec_2)
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

uint8_t calcCurBrightness()
{
  uint8_t hr = my_TZ.hour();
  uint8_t min = my_TZ.minute();
  uint8_t sec = my_TZ.second();
  uint32_t day_sec = hr * (60 * 60) + min * 60 + sec;

  // Normal brightness period
  if(isSecondBetween(day_sec, BRIGHTEN_TRANS_END_SEC, DIMMING_TRANS_START_SEC))
    return NORMAL_BRIGHTNESS;

  // Dim brightness period
  if(isSecondBetween(day_sec, DIMMING_TRANS_END_SEC, BRIGHTEN_TRANS_START_SEC))
    return DIM_BRIGHTNESS;

  // See two-point form of straight line equation in
  // https://www.vedantu.com/maths/equation-of-a-straight-line
  if(isSecondBetween(day_sec, DIMMING_TRANS_START_SEC, DIMMING_TRANS_END_SEC)) // In dimming transition?
    if(DIMMING_TRANS_START_SEC > DIMMING_TRANS_END_SEC && // Dimming period crossing midnight
       day_sec < DIMMING_TRANS_START_SEC) // and we're currently in the next day
      return dimming_m * (day_sec + (60 * 60 * 24) - DIMMING_TRANS_START_SEC) + NORMAL_BRIGHTNESS + 0.5;
    else
      return dimming_m * (day_sec - DIMMING_TRANS_START_SEC) + NORMAL_BRIGHTNESS + 0.5;

  if(isSecondBetween(day_sec, BRIGHTEN_TRANS_START_SEC, BRIGHTEN_TRANS_END_SEC)) // In brightening transition?
    if(BRIGHTEN_TRANS_START_SEC > BRIGHTEN_TRANS_END_SEC && // Brightening period crossing midnight
       day_sec < BRIGHTEN_TRANS_START_SEC) // and we're currently in the next day
      return brighten_m * (day_sec + (60 * 60 * 24) - BRIGHTEN_TRANS_START_SEC) + DIM_BRIGHTNESS + 0.5;
    else
      return brighten_m * (day_sec - BRIGHTEN_TRANS_START_SEC) + DIM_BRIGHTNESS + 0.5;

  // Should not get here
  return 255;
}


int main()
{
    for(int i = 0; i < 48 * 60 * 60; i++)
    {
        printf("[%u:%02u:%02u] brighness = %u\n", my_TZ.hour(), my_TZ.minute(), my_TZ.second(), calcCurBrightness());
        my_TZ.nextSecond();
    }
    return 0;
}
