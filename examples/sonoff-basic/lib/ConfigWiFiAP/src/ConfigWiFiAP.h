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

        void sendTemplate(const String& tmpl);

        void sendForm();
        void sendName();
        void sendMacAddress();
        void sendItems();

        void handleGet();
        void handlePost();
        void respondWith200();
        void respondWith302();

        config_t& config;
        const item_t* items;

        FS& fs;
        wifi_t& wifi;
        DNSServer dnsServer;
        web_server_t webServer;

        std::map<String, item_t> itemMap;

        using send_template_t = std::function<void (void)>;
        std::map<String, send_template_t> templateMap;
};
