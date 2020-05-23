
WiFi_config_t WiFi_config {
    .ssid = "<... your WiFi SSID ...>",
    .pass = "<... your WiFi password ...>",
};

#define SONOFF_BUTTON 0
#define SONOFF_RELAY 12
#define SONOFF_LED1 13

uICALRelay::config_t uICALRelay_config {
    .icalURL = "<... your ICAL URL ...>",
    .hostFingerprint = 0,
    .pollPeriod = 300,
    .statusLedPin = SONOFF_LED1,
    .pushButtonPin = SONOFF_BUTTON,
    .gates = (uICALRelay::gate_t []) {
        { "Porch Light", SONOFF_RELAY },
        { 0, 0 },
    },
};
