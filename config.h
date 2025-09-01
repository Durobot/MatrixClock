#ifndef _CONFIG_H_
#define _CONFIG_H_

// SSID (name) and password of your WiFi network
#define WIFI_SSID     "MyWiFNetwork"
#define WIFI_PASSWORD "WiFi_Password"

// Olson format time zone.
// For the list, see https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
// For the explanation, see https://developer.ibm.com/articles/au-aix-posix/
#define TZ_OLSON "Europe/Chisinau"
// Default Posix time zone string, in case we could not receive it
// from timezoned.rop.nl
#define TZ_POSIX "EET-2EEST,M3.5.0,M10.5.0/3"
// Seconds between NTP (time sync) requests
#define EZTIME_NTP_INTERVAL (60 * 60)

// How many milliseconds the intro screen stays on.
// Must be >= 255 to allow for fade to black.
#define INTO_SCR_MILLIS        3255
// How many milliseconds the WiFi connection results stay on
// after WiFi connection attempt and NTP sync.
// Must be >= 510 to allow for fade to black.
#define WIFI_SCR_RESULT_MILLIS 510

// How many milliseconds digit transition frames are shown
#define DIGIT_TRANS_MILLIS 75

// Duration of fade to black in the intro screen, in milliseconds
#define INTO_FADEOUT_MILLIS 255

// Brightnes (255 is max value) for intro and wifi screens,
// that is before we know current time
#define INTRO_WIFI_BRIGHTNESS 48

// Duration of fade to black in the wifi screen, in milliseconds
#define WIFI_FADEOUT_MILLIS 500

// Render large digits without gradient when screen brightness is equal to
// or below this threshold.
// Set to 0 to always use gradient.
// Set to 255 to never use gradient.
// This is useful when screen brightness is low because dark pixels
// can flicker.
#define LARGE_DIGIT_GRADIENT_THRESHOLD 24

// -- Screen Brightness cycle --
// Consists of normal brightness (bright period) and dim brightess
// with transitional periods between them.

// Screen brightness values
#define DIM_BRIGHTNESS 5
#define NORMAL_BRIGHTNESS 48

// IMPORTANT: 1) periods can't overlap.
//            2) there must be at least 1 second between the end of one
//               period and the start of the next.

// Fully dim (after transition) period start
#define DIM_START_HOUR   21
#define DIM_START_MINUTE 50
#define DIM_START_SECOND 0

// End of fully dim period, when the screen starts
// to transition from dim to normal brightness
#define DIM_END_HOUR   7
#define DIM_END_MINUTE 0
#define DIM_END_SECOND 0

// Fully bright (after transition) period start
#define BRIGHT_START_HOUR   7
#define BRIGHT_START_MINUTE 5
#define BRIGHT_START_SECOND 0

// End of fully bright period, when the screen starts
// to transition from normal to dim brightness
#define BRIGHT_END_HOUR   21
#define BRIGHT_END_MINUTE 45
#define BRIGHT_END_SECOND 0

#endif
