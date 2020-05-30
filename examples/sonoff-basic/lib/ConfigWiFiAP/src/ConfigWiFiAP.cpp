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

/*
PGM_P ConfigWiFiAP::formTmpl() {
    PGM_P tmpl = PSTR(
        "<html>"
        "<head>"
            "<meta name='viewport' content='width=device-width'>"
            "<title>{{name}}</title>"
            "<style>"
                "*{background-color:#292a2b;color:#afafaf;padding:5px;margin:0;box-sizing:border-box;outline:none;font-family:sans-serif;}"
                "h1{font-size:40px;}"
                "p{padding:20px;}"
                "body,label{font-size:20px;}"
                "form{margin:0 10px;overflow:hidden;} "
                "label,a{color:#1bbbe3;}"
                "input{font-size:30px;}"
                "input{background:hsl(0,0%,10%);border:1px solid hsl(0,0%,5%);width:100%;} "
                "input[type=submit]{padding:20px 0;background:hsl(0,0%,20%);}"
                "input[type=submit]:hover{background:hsl(0,0%,25%);}"
                "input[type=submit]:active{background:hsl(0,0%,15%);}"
                "a{text-decoration:none;}"
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
    );
    return tmpl;
}
*/

ConfigWiFiAP::ConfigWiFiAP(config_t& config, item_t* items, FS& fs, wifi_t& wifi)
: config(config)
, items(items)
, fs(fs)
, wifi(wifi)
, dns_server()
, web_server(HTTP_PORT)
{
    template_map["name"] = [&](){this->sendName();};
    template_map["macAddr"] = [&](){this->sendMacAddress();};
    template_map["items"] = [&](){this->sendItems();};

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

    LOG(String("ConfigWiFiAP SSID: ") + ssid);

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

void ConfigWiFiAP::sendName() {
    web_server.sendContent(config.name);
}

void ConfigWiFiAP::sendMacAddress() {
    web_server.sendContent(wifi.macAddress());
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
        web_server.sendContent(html);
    }
}

void ConfigWiFiAP::sendTemplate(const String& tmpl) {
    int cursor = 0;
    int id0, id1;

    while(true) {
        id0 = tmpl.indexOf("{{", cursor);
        if (id0 == -1) {
            web_server.sendContent(tmpl.substring(cursor));
            break;
        }

        web_server.sendContent(tmpl.substring(cursor, id0));

        id1 = tmpl.indexOf("}}", id0);
        if (id1 == -1) {
            web_server.sendContent(tmpl.substring(cursor));
            break;
        }
        {
            String id = tmpl.substring(id0+2, id1);
            auto it = template_map.find(id);
            if (it == template_map.end()) {
               web_server.sendContent(tmpl.substring(id0, id1+2)); 
            }
            else {
                it->second();
            }
        }
        cursor = id1 + 2;
    }
}

void ConfigWiFiAP::sendForm() {
    web_server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    web_server.sendHeader("Pragma", "no-cache");
    web_server.sendHeader("Expires", "-1");

    web_server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    web_server.send(200, "text/html");
    sendTemplate(String(FPSTR(form_tmpl)));
    web_server.sendContent("");
    web_server.client().stop();
}

bool ConfigWiFiAP::setupWebServer() {
    web_server.on("/", HTTP_GET, [&]() {this->handleGet();});
    web_server.on("/", HTTP_POST, [&]() {this->handlePost();});
    web_server.on("/generate_204", HTTP_GET, [&]() {this->respondWith200();});
    web_server.onNotFound([&]() {this->respondWith302();});
    web_server.begin();
    return true;
}

void ConfigWiFiAP::handleGet() {
    if (web_server.hostHeader() != config.hostname) {
        respondWith302();
        return;
    }
    sendForm();
}

void ConfigWiFiAP::handlePost() {
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
    sendForm();
}

void ConfigWiFiAP::respondWith200() {
    web_server.send(200, "text/plain", "");
    web_server.client().stop();
}

void ConfigWiFiAP::respondWith302() {
    //String uri = web_server_t::urlDecode(web_server.uri());
    // Serial.println(String("redirect: ") + uri);

    web_server.sendHeader("Location", String("http://") + config.hostname + "/", true);
    web_server.send(302, "text/plain", "");
    web_server.client().stop();
}
