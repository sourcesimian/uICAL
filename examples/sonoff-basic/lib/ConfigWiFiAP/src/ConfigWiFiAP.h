/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/

class ConfigWiFiAP {
    public:
        struct config_t {
            const char* name;
            const char* ssid_prefix;
            const char* ssid_password;
            int ap_channel;
            int ap_max_connection;
            const char* hostname;
            const char* config_dir;
            const char* config_prefix;
            int value_max_length;
        };

        struct item_t {
            const char* id;
            const char* name;
            const char* max_width;
            int max_length;
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

        bool start();
        void stop();
        void handle();
        String getConfig(String key);

    protected:
        void setConfig(String key, String value);
        String configFilename(String key);

        bool setupWiFiAccessPoint();
        bool setupDnsServer();
        bool setupWebServer();

        void respondWithForm();
        String buildFormLine(String id, String name, String key, int size);
        void receiveFormPost();
        void captivePortalRedirect();

        config_t& config;
        const item_t* items;
        FS& fs;
        wifi_t& wifi;
        DNSServer dns_server;
        web_server_t web_server;

        std::map<String, item_t> item_map;
};
