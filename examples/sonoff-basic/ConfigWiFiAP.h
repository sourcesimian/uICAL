/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/

class ConfigWiFiAP {
    public:
        struct config_t {
            const char* name;
            const char* ssid_prefix;
            const char* hostname;
            const char* config_dir;
            const char* config_prefix;
        };

        struct item_t {
            const char* id;
            const char* name;
            uint8_t size;
            bool secret;
            std::function<bool (String& value)> validate;
        };

        #if defined(ARDUINO_ARCH_ESP8266)
            using wifi_t = ESP8266WiFiClass;
            using web_server_t = ESP8266WebServer;
        #else
            using wifi_t = WiFiClass;
            using web_server_t = WebServer;
        #endif

        ConfigWiFiAP(config_t& config, item_t* items, FS& fs, wifi_t& wifi);

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

        config_t& config;
        const item_t* items;
        FS& fs;
        wifi_t& wifi;
        DNSServer dns_server;
        web_server_t web_server;

        std::map<String, item_t> item_map;
};
