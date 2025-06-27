#include <Arduino.h>

#if not defined(ESP8266) and not defined(ESP32)
#error "Not supported"
#endif  // ! ESP8266 and ! ESP32

#include "FSConfigFile.h"

// global constants
static const uint16_t delayMedium = 250;
static const uint16_t delayLong = 1000;
static const uint32_t interval = 5000;

// major classes
FSConfigFile fsConfigFile;

// global variables
uint32_t currentMillis;
uint32_t previousMillis = 0;
uint16_t interval_count = 0;
bool ledStatus = false;

// Config Data
void config_setup(void) {
    fsConfigFile.add("hostname", fsConfigFile.CHARS, 32);
    fsConfigFile.add("port", fsConfigFile.U16);
    // set defaults, if desired
    fsConfigFile.set("hostname", "");
    fsConfigFile.set("port", static_cast<uint16_t>(2112));
}

void setup() {
    Serial.begin(MY_BAUD);
    delay(delayLong * 5);
    Serial.println("Starting");
    config_setup();
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, ledStatus);

    if (false) {
        //    if (fsConfigFile.exists()) {
        Serial.println("Reading Config");
        // fsConfigFile.read();
    } else {
        if (!fsConfigFile.set("hostname", "victoria.lan"))
            Serial.println("Setting hostname failed");
        if (!fsConfigFile.set("port", static_cast<uint16_t>(4140)))
            Serial.println("Setting port failed");
    }
    previousMillis = millis();
}

void blink(void) {
    Serial.println("Blink");
    ledStatus = !ledStatus;
    digitalWrite(LED_BUILTIN, ledStatus);
}

void loop() {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        blink();
        previousMillis = currentMillis;
        const char *s;
        if (fsConfigFile.get("hostname", s)) {
            Serial.print("hostname: ");
            Serial.println(s);
        } else
            Serial.println("Getting hostname failed");
        uint16_t p;
        if (fsConfigFile.get("port", p)) {
            Serial.print("port: ");
            Serial.println(p);
        } else
            Serial.println("Getting port failed");
        while (true) delay(delayLong);
    }
}
