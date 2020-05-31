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

const int DNS_PORT = 53;
const int HTTP_PORT = 80;

static const char form_tmpl[] PROGMEM = {
    "<html>"
    "<head>"
        "<meta name='viewport' content='width=device-width'>"
        "<title>{{name}}</title>"
        "<style>"
            "*{background-color:#292a2b;color:#afafaf;padding:5px;margin:0;box-sizing:border-box;outline:none;font-family:sans-serif;font-size:18px;}"
            "h1{font-size:35px;}"
            "p{padding:20px;}"
            "form{margin:0 10px;overflow:hidden;} "
            "label,a{color:#1bbbe3;}"
            "input{font-size:25px;}"
            "input{background:hsl(0,0%,10%);border:1px solid hsl(0,0%,5%);width:100%;} "
            "input[type=submit]{padding:20px 0;background:hsl(0,0%,20%);}"
            "input[type=submit]:hover{background:hsl(0,0%,25%);}"
            "input[type=submit]:active{background:hsl(0,0%,15%);}"
            "a{text-decoration:none;font-size:inherit;}"
            ".footer{font-size:13px;text-align:right;}"
        "</style>"
    "</head>"
    "<body>"
        "<h1>{{name}}</h1>"
        "<p>MAC Addr: {{macAddr}}</p>"
        "<form method='post'>"
            "<dl>"
                "{{items}}"
            "</dl>"
            "<input type='submit' value='Update'>"
        "</form>"
        "<p class=footer>Made with<a href=https://github.com/sourcesimian/ConfigWiFiAP>ConfigWiFiAP</a></p>"
    "</body>"
    "</html>"
};


ConfigWiFiAP::ConfigWiFiAP(config_t& config, item_t* items, FS& fs, wifi_t& wifi)
: config(config)
, items(items)
, fs(fs)
, wifi(wifi)
, dnsServer()
, webServer(HTTP_PORT)
{
    templateMap["name"] = [&](){this->sendName();};
    templateMap["macAddr"] = [&](){this->sendMacAddress();};
    templateMap["items"] = [&](){this->sendItems();};

    for (int i=0; i<100; i++) {
        if (items[i].id == 0) break;
        itemMap[items[i].id] = items[i];
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

    LOG(String("ConfigWiFiAP SSID: ") + ssid);

    if (!wifi.softAP(ssid, config.ssid_password, config.ap_channel, 0, config.ap_max_connection)) {
        LOG("ConfigWiFiAP start FAILED ");
        return false;
    }

    LOG(String("ConfigWiFiAP IP address: ") + wifi.softAPIP().toString());
    return true;
}

bool ConfigWiFiAP::setupDnsServer() {
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(DNS_PORT, "*", wifi.softAPIP());
    return true;
}

void ConfigWiFiAP::handle() {
    dnsServer.processNextRequest();
    webServer.handleClient();
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

void ConfigWiFiAP::sendName() {
    webServer.sendContent(config.name);
}

void ConfigWiFiAP::sendMacAddress() {
    webServer.sendContent(wifi.macAddress());
}

void ConfigWiFiAP::sendItems() {
    for (int i=0; i<100; i++) {
        if (items[i].id == 0) break;
        auto item = items[i];
        String value = emptyString;
        if (!item.secret) {
            value = getConfig(item.id);
            value.replace("'", "%27");
        }
        String html =  String() +
            "<dt>"
                "<label for=" + item.id + ">" + item.name + ":</label>"
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
        webServer.sendContent(html);
    }
}

void ConfigWiFiAP::sendTemplate(const String& tmpl) {
    int cursor = 0;
    int id0, id1;

    while(true) {
        id0 = tmpl.indexOf("{{", cursor);
        if (id0 == -1) {
            webServer.sendContent(tmpl.substring(cursor));
            break;
        }

        webServer.sendContent(tmpl.substring(cursor, id0));

        id1 = tmpl.indexOf("}}", id0);
        if (id1 == -1) {
            webServer.sendContent(tmpl.substring(cursor));
            break;
        }
        {
            String id = tmpl.substring(id0+2, id1);
            auto it = templateMap.find(id);
            if (it == templateMap.end()) {
               webServer.sendContent(tmpl.substring(id0, id1+2)); 
            }
            else {
                it->second();
            }
        }
        cursor = id1 + 2;
    }
}

void ConfigWiFiAP::sendForm() {
    webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    webServer.sendHeader("Pragma", "no-cache");
    webServer.sendHeader("Expires", "-1");

    webServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
    webServer.send(200, "text/html");
    sendTemplate(String(FPSTR(form_tmpl)));
    webServer.sendContent("");
    webServer.client().stop();
}

bool ConfigWiFiAP::setupWebServer() {
    webServer.on("/", HTTP_GET, [&]() {this->handleGet();});
    webServer.on("/", HTTP_POST, [&]() {this->handlePost();});
    webServer.on("/generate_204", HTTP_GET, [&]() {this->respondWith200();});
    webServer.onNotFound([&]() {this->respondWith302();});
    webServer.begin();
    return true;
}

void ConfigWiFiAP::handleGet() {
    if (webServer.hostHeader() != config.hostname) {
        respondWith302();
        return;
    }
    sendForm();
}

void ConfigWiFiAP::handlePost() {
    for (int i=0; i< webServer.args(); i++){
        String key = webServer.argName(i);
        auto it = itemMap.find(key);
        if (it == itemMap.end()) {
            continue;
        }
        
        String value = webServer.arg(i);
        value = web_server_t::urlDecode(value);
        auto item = it->second;
        if (item.validate(value)) {
            setConfig(item.id, value);
        }
    }
    sendForm();
}

void ConfigWiFiAP::respondWith200() {
    webServer.send(200, "text/plain", "");
    webServer.client().stop();
}

void ConfigWiFiAP::respondWith302() {
    webServer.sendHeader("Location", String("http://") + config.hostname + "/", true);
    webServer.send(302, "text/plain", "");
    webServer.client().stop();
}
