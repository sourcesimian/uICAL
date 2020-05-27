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

#include <map>

#include <WiFiUDP.h>
#include <DNSServer.h>
#include <FS.h>

#include <ConfigWiFiAP.h>

const byte DNS_PORT = 53;
const byte HTTP_PORT = 80;

/*---------------------------------------------------------------------------*/

/*
static String urlDecode(String str)
{
    auto h2int = [](char c) {
        if (c >= '0' && c <='9'){
            return((unsigned char)c - '0');
        }
        if (c >= 'a' && c <='f'){
            return((unsigned char)c - 'a' + 10);
        }
        if (c >= 'A' && c <='F'){
            return((unsigned char)c - 'A' + 10);
        }
        return(0);
    };

    String encodedString = "";
    char ch, code0, code1;
    for (unsigned i = 0; i < str.length(); i++){
        ch = str.charAt(i);
        if (ch == '+'){
            encodedString += ' ';
        }
        else if (ch == '%') {
            i ++;
            code0 = str.charAt(i);
            i ++;
            code1 = str.charAt(i);
            ch = (h2int(code0) << 4) | h2int(code1);
            encodedString += ch;
    }
    else {
        encodedString += ch;
    }
        yield();
    }
    return encodedString;
}
*/

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

void ConfigWiFiAP::begin() {
    setupWiFiAccessPoint();
    setupDnsServer();
    setupWebServer();
}

void ConfigWiFiAP::setupWiFiAccessPoint() {

    IPAddress local_IP(192, 168, 43, 1);
    
    wifi.disconnect();
    wifi.mode(WIFI_AP);

    if (!wifi.softAPConfig(local_IP, local_IP, IPAddress(255, 255, 255, 0))) {
        Serial.print("Setting soft-AP configuration FAILED ");
    }

    String ssid = config.ssid_prefix + getMacAddress();
    ssid.replace(":", "");

    if (!wifi.softAP(ssid, emptyString, 6, 0, 1)) {
        Serial.print("Setting soft-AP FAILED ");
    }

    Serial.print("ConfigWiFiAP IP address: ");
    Serial.println(getIPAddress());
}


void ConfigWiFiAP::setupDnsServer() {
    dns_server.setErrorReplyCode(DNSReplyCode::NoError);
    dns_server.start(DNS_PORT, "*", wifi.softAPIP());
}

void ConfigWiFiAP::setupWebServer() {
    web_server.on("/", HTTP_GET, [&]() {this->respondWithForm();});
    web_server.on("/", HTTP_POST, [&]() {this->receiveFormPost();});
    web_server.onNotFound([&]() {this->captivePortalRedirect();});
    web_server.begin();
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
        char buf[max_config_value_length];
        int bytesRead = file.readBytes(buf, max_config_value_length - 1);
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
        file.print(value.substring(0, max_config_value_length - 1));
        file.flush();
        file.close();
    }
}

String ConfigWiFiAP::getIPAddress() {
    return wifi.softAPIP().toString();
}

String ConfigWiFiAP::getMacAddress() {
    return wifi.macAddress();
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
        "dl {"
        "display: block;"
        "margin-top: 2em;"
        "margin-bottom: 2em;"
        "margin-left: 1em;"
        "margin-right: 0;"
        "}"
        "</style>"
        "</head>"
        "<body>"
        "<H1>" + String(config.name) + "</H1>";

    content +=
        "<p>MAC Addr: " + getMacAddress() + "</p>";

    content += 
        "<p>"
        "<form method='post'>"
        "<dl>";

    for (int i=0; i<100; i++) {
        if (items[i].id == 0) break;
        auto item = items[i];
        content += String("<dt><label for=") + item.id + ">" + item.name + ":</label></dt> <dd><input"
                          " type=" + (item.secret ? "password" : "text") + 
                          " id=" + item.id + 
                          " name=" + item.id + 
                          " size=" + item.size + 
                          " value='" + (item.secret ? "" : getConfig(item.id)) +  // TODO: URL Encode
                          "'></dd>";
    }

    content +=
        "</dl>"
        "<input type='submit' value='Submit'>"
        "</form>"
        "</p>"
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

    Serial.println(String("redirect: ") + uri);

    web_server.sendHeader("Location", String("http://") + config.hostname + "/", true);
    web_server.send(302, "text/plain", "");
    web_server.client().stop();
}