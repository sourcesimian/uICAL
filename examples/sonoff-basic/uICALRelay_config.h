
const char* g_wifi_ssid = "<your WiFi SSID>";
const char* g_wifi_pass = "<your WiFi password>";

#define SONOFF_BUTTON 0
#define SONOFF_RELAY 12
#define SONOFF_LED1 13

#define POLL_PERIOD 300
#define ICAL_URL "https://calendar.google.com/calendar/ical/<...>/basic.ics"

const uint8_t uICALRelay::statusLedPin = SONOFF_LED1;
const uint8_t uICALRelay::pushButtonPin = SONOFF_BUTTON;

const uICALRelay::Gate uICALRelay::gates[] = {
    { "Porch Light", SONOFF_RELAY },
    { 0, 0 },
};
