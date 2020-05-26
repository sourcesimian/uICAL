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

#include <ConfigIF.h>

const byte DNS_PORT = 53;
const byte HTTP_PORT = 80;

/*---------------------------------------------------------------------------*/
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

static auto wifi = WiFi;
const char* hostname = "uicalrelay.config";

/*---------------------------------------------------------------------------*/
ConfigIF::ConfigIF(uICALRelay_config_t& config)
: config(config)
, fs(SPIFFS)
, dns_server()
, web_server(HTTP_PORT)
{}

void ConfigIF::begin() {
    setupWiFiAccessPoint();
    setupDnsServer();
    setupWebServer();
}

void ConfigIF::setupWiFiAccessPoint() {
    wifi.disconnect();

    IPAddress local_IP(192, 168, 43, 1);
    
    Serial.println();

    Serial.print("Setting soft-AP configuration ... ");
    Serial.println(wifi.softAPConfig(local_IP, local_IP, IPAddress(255, 255, 255, 0)) ? "Ready" : "Failed!");

    wifi.mode(WIFI_AP);

    String ssid = "uICALRelay-" + getMacAddress();
    ssid.replace(":", "");

    Serial.print("Setting soft-AP ... ");
    Serial.println(wifi.softAP(ssid, emptyString, 6, 0, 1) ? "Ready" : "Failed!");

    Serial.print("Soft-AP IP address = ");
    Serial.println(getIPAddress());
}


void ConfigIF::setupDnsServer() {
    dns_server.setErrorReplyCode(DNSReplyCode::NoError);
    dns_server.start(DNS_PORT, "*", wifi.softAPIP());
}

void ConfigIF::setupWebServer() {
    web_server.on("/", HTTP_GET, [&]() {this->respondWithForm();});
    web_server.on("/", HTTP_POST, [&]() {this->receiveFormPost();});
    web_server.onNotFound([&]() {this->captivePortalRedirect();});
    web_server.begin();
}

void ConfigIF::handle() {
    dns_server.processNextRequest();
    web_server.handleClient();
}

String ConfigIF::configFilename(String key) {
    return String("/config-") + key;
}

String ConfigIF::getConfig(String key) {
    File file = fs.open(configFilename(key), "r");
    if (file) {
        char buf[max_config_value_length];
        int bytesRead = file.readBytes(buf, max_config_value_length - 1);
        buf[bytesRead] = 0;
        return String(buf);
    }
    return String();
}

void ConfigIF::setConfig(String key, String value) {
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

String ConfigIF::getIPAddress() {
    return wifi.softAPIP().toString();
}

String ConfigIF::getMacAddress() {
    return wifi.macAddress();
}

void ConfigIF::respondWithForm() {
    Serial.println(String("respondWithForm: ") + web_server.hostHeader());

    if (web_server.hostHeader() != hostname) {
        captivePortalRedirect();
        return;
    }

    String content;
    
    content +=
        "<html>"
        "<head><title>uICAL Relay</title></head>"
        "<body>"
        "<H1>uICAL Relay</H1>";

    content +=
        "<p>MAC Addr: " + getMacAddress() + "</p>";

    content += 
        "<p>"
        "<form method='post'>"
        "<dl>";

    content += buildFormLine("wifissid", "WiFi SSID", "wifissid", 20);
    content += buildFormLine("wifipass", "WiFi Password", emptyString, 20);
    content += buildFormLine("icalurl", "ICAL URL", "icalurl", 120);
    content += buildFormLine("fingerprint", "Host Fingerprint", "fingerprint", 60);
    content += buildFormLine("poll", "Poll Period (s)", "poll", 5);
    for (int i=0; i<100; i++) {
        if (!config.gatePins[i].id) {
            break;
        }
        String id = String("gate-") + config.gatePins[i].id;
        content += buildFormLine(id, String("Gate \"") + config.gatePins[i].id + "\" Summary" , id, 40);
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

String ConfigIF::buildFormLine(String id, String name, String key, int size) {
    String value;
    const char* type;
    if (!key.isEmpty()) {
        type = "text";
        value = getConfig(key);
    }
    else {
        type = "password";
        value = "";
    }
    return String("<dt><label for=") + id + ">" + name + ":</label></dt> <dd><input"
                  " type=" + type + 
                  " id=" + id + 
                  " name=" + id + 
                  " size=" + size + 
                  " value='" + value + 
                  "'></dd>";
}


void ConfigIF::receiveFormPost() {

    String post_body = web_server.arg("plain");

    int from = 0;
    while (true) {
        int split = post_body.indexOf('=', from);
        if (split == -1) {
            break;
        }

        int token = post_body.indexOf('&', from);
        if (token == -1) {
            token = post_body.length();
        }

        String key = post_body.substring(from, split);
        String value = post_body.substring(split + 1, token);
        value = urlDecode(value);
        from = token + 1;

        if (key == "wifipass") {
            if (!value.isEmpty()) {
                setConfig(key, value);
            }
        }
        else if (key == "poll") {
            int poll = value.toInt();
            if (poll >= 5) {
                setConfig(key, value);
            }
        }
        else {
            setConfig(key, value);
        }
    }
    respondWithForm();
}




bool ConfigIF::isCaptivePortal() {
    return false;
}

void ConfigIF::captivePortalRedirect() {
    Serial.println(String("captivePortalRedirect: ") + web_server.hostHeader());

    Serial.println("Request redirected to captive portal");
    web_server.sendHeader("Location", String("http://") + hostname + "/", true);
    web_server.send(302, "text/plain", "");
    web_server.client().stop();
}