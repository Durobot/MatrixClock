/*
 * Build for Wemos Lolin32 (CPU frequency 240 MHz).
 * Compiles with ESP32 platform by Espressif v2.0.x
 * Does not compile with version 3.x
 */

#include <soc/gpio_struct.h> // For PxMatrix
#include <PxMatrix.h>
#include <WiFi.h>
#include <ezTime.h>

#include "common.h"
#include "config.h"

// Pins for LED MATRIX
#ifdef ESP32

#define P_LAT 22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 2
hw_timer_t* timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

#endif

#ifdef ESP8266

#include <Ticker.h>
Ticker display_ticker;
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2

#endif

#define DISPLAY_ROW_PATTERN 16

#include "Application.h"


// This defines the 'on' time of the display is us. The larger this number,
// the brighter the display. If too large the ESP will crash
uint8_t display_draw_time = 30; //30-60 is usually fine

PxMATRIX display(SCREEN_WIDTH, SCREEN_HEIGHT, P_LAT, P_OE, P_A, P_B, P_C, P_D);

// ------------------------------------------

Application app;

// ezTime
Timezone my_TZ;

// ------------------------------------------

#ifdef ESP8266
// ISR for display refresh
void display_updater()
{
  display.display(display_draw_time);
}
#endif

#ifdef ESP32
void IRAM_ATTR display_updater()
{
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  display.display(display_draw_time);
  portEXIT_CRITICAL_ISR(&timerMux);
}
#endif


void display_update_enable(bool is_enable)
{
#ifdef ESP8266
  if(is_enable)
    display_ticker.attach(0.001, display_updater);
  else
    display_ticker.detach();
#endif

#ifdef ESP32
  if(is_enable)
  {
    timer = timerBegin(0, 80, true); // Old version of esp32 platform (v2.0.x)
    //timer = timerBegin(1000000); //(80000000); // New version of esp32 platform (x3.x - doesn't work)
    timerAttachInterrupt(timer, &display_updater, true);
    timerAlarmWrite(timer, 2000, true);
    timerAlarmEnable(timer);
  }
  else
  {
    timerDetachInterrupt(timer);
    timerAlarmDisable(timer);
  }
#endif
}

bool wifi_connect()
{
  Serial.print("Connecting to WiFi network ");
  Serial.println(WIFI_SSID);

  if(WiFi.isConnected())
    WiFi.disconnect();

  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  for(int i = 0; i < 8; i++) // 8 times 500 milliseconds = 4 seconds
  {
      delay(500);
      if(WiFi.status() == WL_CONNECTED)
      {
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        return true;
      }
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connection timed out");
  return false;
}

void setup()
{
/*--------------------
  Serial.begin(115200);
  // Define your display layout here, e.g. 1/8 step, and optional SPI pins begin(row_pattern, CLK, MOSI, MISO, SS)
  //display.begin(8);
  display.begin(16);
  display.setFastUpdate(true);
  //display.begin(8, 14, 13, 12, 4);
----------------------*/

  // Define multiplex implemention here {BINARY, STRAIGHT} (default is BINARY)
  //display.setMuxPattern(BINARY);

  // Set the multiplex pattern {LINE, ZIGZAG,ZZAGG, ZAGGIZ, WZAGZIG, VZAG, ZAGZIG} (default is LINE)
  //display.setScanPattern(LINE);

  // Rotate display
  //display.setRotate(true);

  // Flip display
  //display.setFlip(true);

  // Helps to reduce display update latency on larger displays
  //display.setFastUpdate(true);

  // Control the minimum color values that result in an active pixel
  //display.setColorOffset(5, 5,5);

  // Set the multiplex implemention {BINARY, STRAIGHT} (default is BINARY)
  //display.setMuxPattern(BINARY);

  // Set the color order {RRGGBB, RRBBGG, GGRRBB, GGBBRR, BBRRGG, BBGGRR} (default is RRGGBB)
  //display.setColorOrder(RRGGBB);

  // Set the time in microseconds that we pause after selecting each mux channel
  // (May help if some rows are missing / the mux chip is too slow)
  //display.setMuxDelay(0,1,0,0,0);

  // Set the number of panels that make up the display area width (default is 1)
  //display.setPanelsWidth(2);

  // Set the brightness of the panels (default is 255)
  //display.setBrightness(50);

  // Set driver chip type
  //display.setDriverChip(FM6124);

/*
  display.clearDisplay();
  display.setTextColor(myCYAN);
  display.setCursor(2, 0);
  display.print("Pixel");
  display.setTextColor(myMAGENTA);
  display.setCursor(2, 8);
  display.print("Time");
*/

  Serial.begin(115200);

  delay(10);

  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  
  // Define your display layout here, e.g. 1/8 step, and optional SPI pins begin(row_pattern, CLK, MOSI, MISO, SS)
  display.begin(DISPLAY_ROW_PATTERN);
  display.flushDisplay();
  display.setTextWrap(false);

  display_update_enable(true);

  // -- ezTime --
  // Uncomment the line below to see what it does behind the scenes
  //setDebug(INFO);
  // By default, ezTime is set to poll pool.ntp.org about every 30 minutes
  // (works if events() function is called periodically).
  // Let's change this interval to 1 hour.
  setInterval(EZTIME_NTP_INTERVAL);
}

/*
int16_t x=0, dx=1;
unsigned long ms_current  = 0;
unsigned long ms_previous = 0;

#define FADE_FELAY_MS 50
unsigned long ms_since_bright_change = 0;
int8_t dbright = -1;
uint8_t brightness = 255;

#define ANIMATION_DELAY 20
unsigned long ms_since_anim_step = 0;
*/

void loop()
{
  app.update();
  // ezTime
  events();

/*
  ms_previous = ms_current;
  ms_current = millis();
  unsigned long ms_delta = ms_current - ms_previous;
*/
/*
  // Cycle brightness
  ms_since_bright_change += ms_delta;
  if(ms_since_bright_change >= FADE_FELAY_MS)
  {
    ms_since_bright_change = 0;
    brightness += dbright;
    // Set the brightness of the panels (default is 255)
    display.setBrightness(brightness);
    if(dbright < 0)
    {
      if(brightness == 0)
        dbright = 5;
    }
    else
      if(brightness == 255)
        dbright = -5;
  }

  // Animate
  ms_since_anim_step += ms_delta;
  if(ms_since_anim_step > ANIMATION_DELAY)
  {
    ms_since_anim_step = 0;
    if(x + dx >= display.width() || x + dx < 0)
      dx = -dx;
    x += dx;

//    display.clearDisplay();
    display.fillScreen(myBLACK);
    display.drawLine(x, 0, display.width() - x - 1, display.height() - 1, myMAGENTA);
    //display.drawPixelRGB888();
    display.showBuffer();
  }
*/

/*
  RGB888 p = { .r = 255, .g = 0, .b = 0 };
  unsigned long start_time = micros();
  for(int i = 0; i < 1000; i++)
    for(int j = 0; j < MATRIX_WIDTH * MATRIX_HEIGHT; j++)
      mix.setNextPixel(p);
  unsigned long end_time = micros();
  Serial.print("1000 screens worth of setNextPixel() calls took ");
  Serial.print(end_time - start_time);
  Serial.println(" microseconds");
*/
/*
  ms_since_anim_step += ms_delta;
  if(ms_since_anim_step > ANIMATION_DELAY)
  {
    ms_since_anim_step = 0;
    if(x + dx >= display.width() || x + dx < 0)
      dx = -dx;
    x += dx;

//    display.clearDisplay();
    display.fillScreen(myBLACK);
    //display.drawFastVLine(x, 0, MATRIX_HEIGHT - 1, myMAGENTA);
    for(uint16_t y = 0; y < MATRIX_HEIGHT; y++)
    {
      //display.drawPixel(x, y, myMAGENTA);
      display.drawPixelRGB888(x, y, 255, 0, 255);
    }
    display.showBuffer(); 
  }
*/
}
