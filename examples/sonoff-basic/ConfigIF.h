/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/

struct gatePin_t {
    const char* id;
    const uint8_t pin;
};

struct uICALRelay_config_t {
    uint8_t statusLedPin;
    uint8_t pushButtonPin;
    gatePin_t* gatePins;
};


class ConfigIF {
    public:
        ConfigIF(uICALRelay_config_t& config);

        void begin();
        void handle();
        String getConfig(String key);

    protected:
        void setConfig(String key, String value);
        String configFilename(String key);

        void setupWiFiAccessPoint();
        void setupDnsServer();
        void setupWebServer();

        void respondWithForm();
        String buildFormLine(String id, String name, String key, int size);
        void receiveFormPost();
        bool isCaptivePortal();
        void captivePortalRedirect();

        String getIPAddress();
        String getMacAddress();

        const size_t max_config_value_length = 255;

        uICALRelay_config_t& config;
        FS& fs;
        
        DNSServer dns_server;
        #if defined(ARDUINO_ARCH_ESP8266)
            ESP8266WebServer web_server;
        #else
            WebServer web_server;
        #endif
};