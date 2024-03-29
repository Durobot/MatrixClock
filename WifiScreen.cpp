#include <Arduino.h>
#include <WiFi.h>
#include <PxMatrix.h>
#include <ezTime.h>

#include "common.h"
#include "config.h"
#include "Application.h"
#include "WifiScreen.h"
#include "GlyphRenderer.h"

extern Application app;
extern PxMATRIX display;
//extern const char* wifi_ssid;
extern Timezone my_TZ;

bool wifi_connect(); // In MatrixClock.ino
void display_update_enable(bool is_enable);

// Fadeout slope, see https://www.mathsisfun.com/equation_of_line.html
const float wifi_fadeout_m = -(float)INTRO_WIFI_BRIGHTNESS / (float)WIFI_FADEOUT_MILLIS;

WifiScreen::WifiScreen(unsigned int scr_id) : Screen(scr_id)
{}

WifiScreen::~WifiScreen()
{}

void WifiScreen::update(unsigned long frame_millis, unsigned long prev_frame_millis)
{
  if(this->start_millis == 0)
  {
    this->start_millis = frame_millis; // Initialize
    display.clearDisplay();
    display.setBrightness(INTRO_WIFI_BRIGHTNESS); // Set the brightness of the panel (max is 255)

    // -- Attempt WiFi connection --
    //
    // Must disable screen before trying to connect to wifi
    display_update_enable(false);
    bool wifi_result = wifi_connect();
    // otherwise the microcontroller is going to crash
    display_update_enable(true);

    // -- Display WiFi connection result --
    //
    const struct RGB888 yellow_clr = { .r = 192, .g = 192, .b = 0 };
    // SSID string X position - centered
    int ssid_x_pos = (SCREEN_WIDTH >> 1) - ((sizeof WIFI_SSID - 1) >> 1) * 6;
    if(ssid_x_pos < 0)
      ssid_x_pos = 0;
    if(wifi_result)
    {
      Serial.print("Wifi ");
      Serial.print(WIFI_SSID);
      Serial.println(" connected");

      const struct RGB888 green_clr = { .r = 22, .g = 192, .b = 0 };
      GlyphRenderer::drawSmallString("Wifi", 20, 0, green_clr);
      GlyphRenderer::drawSmallString(WIFI_SSID, ssid_x_pos, 8, yellow_clr);
      GlyphRenderer::drawSmallString("connected", 4, 16, green_clr);
      display.drawPixelRGB888(60, 22, green_clr.r, green_clr.g, green_clr.b);
      const struct RGB888 grey_clr = { .r = 168, .g = 168, .b = 168 };
      GlyphRenderer::drawSmallString("Time", 2, 24, grey_clr);
      GlyphRenderer::drawSmallString("sync.", 32, 24, grey_clr);
      display.drawPixelRGB888(61, 30, grey_clr.r, grey_clr.g, grey_clr.b);

      // -- ezTime --
      // Provide official timezone names
      // https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
      // setLocation() gets Posix TZ string from timezoned.rop.nl ,
      // which is ezTime's own service.
      if(!my_TZ.setLocation(F(TZ_OLSON)))
      {
        Serial.println("Could not get Posix time string from timezoned.rop.nl:");
        Serial.println(errorString().c_str());
        Serial.println("Trying again..");
        if(!my_TZ.setLocation(F(TZ_OLSON)))
        {
          Serial.println("Failed again:");
          Serial.println(errorString().c_str());
          Serial.println("Setting default Posix string:");
          Serial.println(TZ_POSIX);
          my_TZ.setPosix(F(TZ_POSIX));
        }
        else
        {
          Serial.print("Worked this time. Posix time string from timezoned.rop.nl = ");
          Serial.println(my_TZ.getPosix().c_str());
        }
      }
      else
      {
        Serial.print("Posix time string from timezoned.rop.nl = ");
        Serial.println(my_TZ.getPosix().c_str());
      }

      // Uncomment the line below to see what it does behind the scenes
      // setDebug(INFO);
      waitForSync(); // Shouldn't take long :)

      this->wifi_result_millis = millis(); // wifi_connect() + waitForSync() normally take several seconds

      // Setting the Posix string directly removes dependency on timezoned.rop.nl,
      // but if TZ definition changes, we're stuck with the old definition.
      //my_TZ.setPosix(F("EET-2EEST,M3.5.0,M10.5.0/3"));

      Serial.print(F(TZ_OLSON));
      Serial.print(F(" : "));
      Serial.println(my_TZ.dateTime());
    }
    else
    {
      this->wifi_result_millis = millis(); // wifi_connect() normally takes several seconds
      
      Serial.print("Wifi ");
      Serial.print(WIFI_SSID);
      Serial.println(" connection failed");

      const struct RGB888 red_clr = { .r = 192, .g = 22, .b = 0 };
      GlyphRenderer::drawSmallString("Wifi", 20, 0, red_clr);
      GlyphRenderer::drawSmallString(WIFI_SSID, ssid_x_pos, 8, yellow_clr);
      GlyphRenderer::drawSmallString("connection", 1, 16, red_clr);
      GlyphRenderer::drawSmallString("failed", 14, 24, red_clr);
    }

    return;
  } // if(this->start_millis == 0)

  unsigned long after_wifi_result_millis = frame_millis - this->wifi_result_millis;
  if(WiFi.isConnected())
  {
    if(after_wifi_result_millis >= WIFI_SCR_RESULT_MILLIS)
    {
      this->start_millis = this->wifi_result_millis = 0;
      app.switchToScreen(SCR_CLOCK); // Proceed to the clock screen
      return;
    }

    // Fade to black
    if(after_wifi_result_millis >= WIFI_SCR_RESULT_MILLIS - WIFI_FADEOUT_MILLIS)
    {
      // see https://www.mathsisfun.com/equation_of_line.html
      display.setBrightness(wifi_fadeout_m * (after_wifi_result_millis - (WIFI_SCR_RESULT_MILLIS - WIFI_FADEOUT_MILLIS))
        + INTRO_WIFI_BRIGHTNESS);
      return;
    }
  }
  else // WiFi not connected, show the error longer than WIFI_SCR_RESULT_MILLIS
  {
    if(after_wifi_result_millis >= WIFI_SCR_RESULT_MILLIS + 2000)
    {
      this->start_millis = this->wifi_result_millis = 0;
      app.switchToScreen(SCR_INTRO); // Go back to intro screen
      return;
    }

    // Fade to black
    if(after_wifi_result_millis >= WIFI_SCR_RESULT_MILLIS + 2000 - WIFI_FADEOUT_MILLIS)
    {
      display.setBrightness(wifi_fadeout_m * (after_wifi_result_millis - (WIFI_SCR_RESULT_MILLIS + 2000 - WIFI_FADEOUT_MILLIS))
        + INTRO_WIFI_BRIGHTNESS);
      return;
    }
  }
}
