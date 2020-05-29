/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
#
# uICALRelay device config for:
#   Sonoff Basic RF R2 Power v1.0
############################################################################*/

#define SONOFF_BUTTON 0
#define SONOFF_RELAY 12
#define SONOFF_LED1 13

device_config_t device_config {
    .statusLedPin = SONOFF_LED1,
    .pushButtonPin = SONOFF_BUTTON,
    .gatePins = (device_gate_pin_t []) {
        { "relay", SONOFF_RELAY, true },
        { 0, 0, 0 },
    },
};

const char* ntp_host = "pool.ntp.org";

ConfigWiFiAP::config_t config_ap {
    .name = "Sonoff Basic",
    .ssid_prefix = "SonoffBasic-",
    .ssid_password = 0,
    .ap_channel = 6,
    .ap_max_connection = 1,
    .hostname = "sonoff-basic.config",
    .config_dir = "/sonoff-basic",
    .config_prefix = "cfg-",
    .value_max_length = 255,
};

ConfigWiFiAP::item_t config_ap_items[] {
    { "wifissid", "WiFi SSID", "15em", 30, false, [](String& v){
            return !v.isEmpty();
        }
    },
    { "wifipass", "WiFi Password", "15em", 30, true, [](String& v){
            return !v.isEmpty();
        }
    },
    { "icalurl", "ICAL URL", "40em", config_ap.value_max_length, false, [](String& v){
            return !v.isEmpty();
        }
    },
    { "fingerprint", "Host Fingerprint", "30em", config_ap.value_max_length, false, [](String& v){
            return true;
        }
    },
    { "poll", "Poll Period (s)", "3em", 5, false, [](String& v){
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
