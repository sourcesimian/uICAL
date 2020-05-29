/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/

#if defined(ARDUINO_ARCH_ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESP8266WebServer.h>
#else
    #include <Arduino.h>
    #include <WebServer.h>
#endif

#include <WiFiUDP.h>
#include <DNSServer.h>
#include <FS.h>

#include <map>

#include <ConfigWiFiAP.h>

#if defined(DEBUG)
    #define LOG(...) Serial.println(__VA_ARGS__)
#else
    #define LOG(...) while(false){}
#endif

const byte DNS_PORT = 53;
const byte HTTP_PORT = 80;

/*---------------------------------------------------------------------------*/
ConfigWiFiAP::ConfigWiFiAP(config_t& config, item_t* items, FS& fs, wifi_t& wifi)
: config(config)
, items(items)
, fs(fs)
, wifi(wifi)
, dns_server()
, web_server(HTTP_PORT)
{
    for (int i=0; i<100; i++) {
        if (items[i].id == 0) break;
        item_map[items[i].id] = items[i];
    }
}

bool ConfigWiFiAP::start() {
    if (!setupWiFiAccessPoint()) return false;
    if (!setupDnsServer()) return false;
    if (!setupWebServer()) return false;
    return true;
}

void ConfigWiFiAP::stop() {
    wifi.disconnect();
    wifi.mode(WIFI_OFF);
}

bool ConfigWiFiAP::setupWiFiAccessPoint() {

    IPAddress local_IP(192, 168, 43, 1);
    
    wifi.disconnect();
    wifi.mode(WIFI_AP);

    if (!wifi.softAPConfig(local_IP, local_IP, IPAddress(255, 255, 255, 0))) {
        LOG("ConfigWiFiAP config FAILED ");
        return false;
    }

    String ssid = config.ssid_prefix + wifi.macAddress();
    ssid.replace(":", "");

    if (!wifi.softAP(ssid, config.ssid_password, config.ap_channel, 0, config.ap_max_connection)) {
        LOG("ConfigWiFiAP start FAILED ");
        return false;
    }

    LOG(String("ConfigWiFiAP IP address: ") + wifi.softAPIP().toString());
    return true;
}

bool ConfigWiFiAP::setupDnsServer() {
    dns_server.setErrorReplyCode(DNSReplyCode::NoError);
    dns_server.start(DNS_PORT, "*", wifi.softAPIP());
    return true;
}

bool ConfigWiFiAP::setupWebServer() {
    web_server.on("/", HTTP_GET, [&]() {this->respondWithForm();});
    web_server.on("/", HTTP_POST, [&]() {this->receiveFormPost();});
    web_server.onNotFound([&]() {this->captivePortalRedirect();});
    web_server.begin();
    return true;
}

void ConfigWiFiAP::handle() {
    dns_server.processNextRequest();
    web_server.handleClient();
}

String ConfigWiFiAP::configFilename(String key) {
    return String(config.config_dir) + "/" + config.config_prefix + key;
}

String ConfigWiFiAP::getConfig(String key) {
    File file = fs.open(configFilename(key), "r");
    if (file) {
        char buf[config.value_max_length];
        int bytesRead = file.readBytes(buf, config.value_max_length - 1);
        buf[bytesRead] = 0;
        return String(buf);
    }
    return String();
}

void ConfigWiFiAP::setConfig(String key, String value) {
    String path = configFilename(key);
    if (fs.exists(path)) {
        fs.remove(path);
    }
    File file = fs.open(path, "w");
    if (file) {
        file.print(value.substring(0, config.value_max_length - 1));
        file.flush();
        file.close();
    }
}

void ConfigWiFiAP::respondWithForm() {
    if (web_server.hostHeader() != config.hostname) {
        captivePortalRedirect();
        return;
    }

    String content;
    
    content +=
        "<html>"
        "<head>"
            "<meta name='viewport' content='width=device-width'>"
            "<title>" + String(config.name) + "</title>"
            "<style>"
                "body{background-color: #d0e0f0;} "
                "form{background-color: #ffffff; margin: 1em; padding: 1em; overflow: hidden;} "
                "input{background-color: #f4f8fc;width: 100%;} "
            "</style>"
        "</head>"
        "<body>"
        "<h1>" + String(config.name) + "</h1>";

    content +=
        "<p>MAC Addr: " + wifi.macAddress() + "</p>";

    content += 
        "<form method='post'>"
        "<dl>";

    for (int i=0; i<100; i++) {
        if (items[i].id == 0) break;
        auto item = items[i];
        String value = emptyString;
        if (!item.secret) {
            value = getConfig(item.id);
            value.replace("'", "%27");
        }
        content += String("<dt>"
                              "<label for=") + item.id + ">" + item.name + ":</label>"
                          "</dt>"
                          "<dd>"
                              "<input"
                              " type=" + (item.secret ? "password" : "text") + 
                              " id=" + item.id + 
                              " name=" + item.id + 
                              " maxlength=" + item.max_length + 
                              " style='max-width: " + item.max_width + "'"
                              " value='" + value + "'>"
                          "</dd>";
    }

    content +=
        "</dl>"
        "<input type='submit' value='Update'>"
        "</form>"
        "</body>"
        "</html>";
    
    web_server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    web_server.sendHeader("Pragma", "no-cache");
    web_server.sendHeader("Expires", "-1");

    web_server.send(200, "text/html", content);
}

void ConfigWiFiAP::receiveFormPost() {
    for (int i=0; i< web_server.args(); i++){
        String key = web_server.argName(i);
        auto it = item_map.find(key);
        if (it == item_map.end()) {
            continue;
        }
        
        String value = web_server.arg(i);
        value = web_server_t::urlDecode(value);
        auto item = it->second;
        if (item.validate(value)) {
            setConfig(item.id, value);
        }

    }
    respondWithForm();
}

void ConfigWiFiAP::captivePortalRedirect() {
    String uri = web_server_t::urlDecode(web_server.uri());

    // Serial.println(String("redirect: ") + uri);

    web_server.sendHeader("Location", String("http://") + config.hostname + "/", true);
    web_server.send(302, "text/plain", "");
    web_server.client().stop();
}
