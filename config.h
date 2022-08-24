#ifndef _CONFIG_H_
#define _CONFIG_H_

// SSID (name) and password of your WiFi network
#define WIFI_SSID     "MyWiFNetwork"
#define WIFI_PASSWORD "WiFi Password"

// Olson format time zone.
// For the list, see https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
// For the explanation, see https://developer.ibm.com/articles/au-aix-posix/
#define TZ_OLSON "Europe/Chisinau"
// Seconds between NTP (time sync) requests
#define EZTIME_NTP_INTERVAL (60 * 60)
// How many milliseconds the intro screen stays on.
// Must be >= 255 to allow for fade to black.
#define INTO_SCR_MILLIS        3255
// How many milliseconds the WiFi connection results stay on
// after WiFi connection attempt and NTP sync.
// Must be >= 510 to allow for fade to black.
#define WIFI_SCR_RESULT_MILLIS 510


#endif