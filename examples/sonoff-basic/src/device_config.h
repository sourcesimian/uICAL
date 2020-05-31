/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
#
# uICALRelay device config for:
#   Sonoff basic (Sonoff RF R2 POWER V1.0) WiFi Smart Switch
############################################################################*/

#define SONOFF_BUTTON 0
#define SONOFF_RELAY 12
#define SONOFF_LED1 13

device_config_t device_config {
    .status_led_pin = SONOFF_LED1,
    .push_button_pin = SONOFF_BUTTON,
    .gate_pins = (device_gate_pin_t []) {
        { "relay", SONOFF_RELAY, true },
        { 0, 0, 0 },
    },
};

const char* NTP_HOST = "pool.ntp.org";

ConfigWiFiAP::config_t config_ap {
    .name = "uICAL Timeswitch",
    .ssid_prefix = "uICAL-Timeswitch-",
    .ssid_password = 0,
    .ap_channel = 6,
    .ap_max_connection = 1,
    .hostname = "uical-timeswitch.config",
    .config_dir = "/uical",
    .config_prefix = "cfg-",
    .value_max_length = 255,
};

ConfigWiFiAP::item_t config_ap_items[] {
    { "wifissid", "WiFi Name", "15em", 30, false, [](String& v){
            return true;
        }
    },
    { "wifipass", "WiFi Password", "15em", 30, true, [](String& v){
            return !v.isEmpty();
        }
    },
    { "icalurl", "Calendar ICAL URL", "40em", config_ap.value_max_length, false, [](String& v){
            return !v.isEmpty();
        }
    },
    { "fingerprint", "Host Fingerprint (SHA1)", "30em", config_ap.value_max_length, false, [](String& v){
            return true;
        }
    },
    { "poll", "Calendar Update Period (s)", "4em", 6, false, [](String& v){
            if (v.isEmpty()) { return false; }
            return v.toInt() >= 5;
        }
    },
    { "relay", "Relay Event Summary", "15em", config_ap.value_max_length, false, [](String& v){
            return !v.isEmpty();
        }
    },
    { 0 },
};
