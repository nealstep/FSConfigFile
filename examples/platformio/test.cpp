#include <Arduino.h>

#if not defined(ESP8266) and not defined(ESP32)
#error "Not supported"
#endif  // ! ESP8266 and ! ESP32

#include "FSConfigFile.h"


// global constants
static const uint16_t delayMedium = 250;
static const uint16_t delayLong = 1000;
static const uint32_t interval = 30000;

// major classes
FSConfigFile fsConfigFile;
FSConfigFile fsConfigFile2;

// global variables
uint32_t currentMillis;
uint32_t previousMillis = 0;
uint16_t interval_count = 0;
bool ledStatus = false;

void setup() {
    Serial.begin(MY_BAUD);
    Serial.println("Starting");
    pinMode(LED_BUILTIN, OUTPUT);
    if (!fsConfigFile.read()) {
        Serial.println("Config file failed to open");
        while(true);
    }
    fsConfigFile.config["test_s"] = "sting";
    fsConfigFile.config["test_u"] = (uint8_t) 45;
    if (!fsConfigFile.write()) {
        Serial.println("Config file failed to open");
        while(true);
    }

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
        fsConfigFile2.read();
        Serial.print("test_s: ");
        const char *s = fsConfigFile2.config["test_s"];
        Serial.println(s);
        Serial.print("test_u: ");
        uint8_t u = fsConfigFile2.config["test_u"];
        Serial.println(u);
    }
}
