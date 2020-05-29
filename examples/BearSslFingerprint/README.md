# BearSSL Fingerprint Lookup
When using the [BearSSL WiFi Classes](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/bearssl-client-secure-class.html) I encountered an [issue](https://github.com/esp8266/Arduino/issues/6209) with how the library handles the SHA1 fingerprint passed to [setFingerprint()](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/bearssl-client-secure-class.html#setfingerprint-const-uint8-t-fp-20-setfingerprint-const-char-fpstr).

One would expect that the following would work, but it does not:
```
openssl s_client -connect ${HOST}:${PORT} </dev/null 2>/dev/null | openssl x509 -fingerprint -sha1 -noout -in /dev/stdin
```

A quick and dirty way to establish the fingerprint that the library requires is to build BearSSLFingerprint.cpp (with `DEBUG_ESP_SSL` and `DEBUG_ESP_PORT` defined) and run it on your device. You will need to define the following build flags:
* `WIFI_NAME`
* `WIFI_PASS`
* `URL_TO_FETCH`

Then monitor the serial output and you will see the fingerprint that BearSSL expects, e.g:

```
...
BSSL:insecure_end_chain: Received cert FP doesn't match
BSSL:insecure_end_chain: expected ea e0 8e 6a ac 0f ba 1c 8c f1 16 e7 41 89 bd 4b 8e 18 27 43 
BSSL:insecure_end_chain: received f3 c2 ff 9d 10 7e b8 fc 96 13 da de e8 43 1d 2f 65 ae 2c fa 
BSSL:_wait_for_handshake: failed
...
```
