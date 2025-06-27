#include <Arduino.h>

#if not defined(ESP8266) and not defined(ESP32)
#error "Not supported"
#endif  // ! ESP8266 and ! ESP32
#include <ArduinoLog.h>

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
    Log.traceln("config_setup");
    Log.verboseln("Creating fields");
    fsConfigFile.add("hostname", fsConfigFile.CHARS, 32);
    fsConfigFile.add("port", fsConfigFile.U16);
    // set defaults, if desired
    Log.verboseln("Setting defaults");
    fsConfigFile.set("hostname", "");
    fsConfigFile.set("port", static_cast<uint16_t>(2112));
}

void setup() {
    Serial.begin(MY_BAUD);
    Log.begin(LOG_LEVEL_VERBOSE, &Serial, true);
    delay(delayLong * 5);
    Log.traceln("Starting");
    config_setup();
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, ledStatus);

    if (fsConfigFile.exists()) {
        Log.verboseln("Reading config");
        fsConfigFile.read();
    } else {
        // change hostname so we know not defaults
        fsConfigFile.set("hostname", "tester");
        Log.verboseln("Writing default config");
        fsConfigFile.write();
    }
    previousMillis = millis();
}

void blink(void) {
    Log.traceln("blink");
    ledStatus = !ledStatus;
    digitalWrite(LED_BUILTIN, ledStatus);
}

void loop() {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        const char *s;
        uint16_t p;

        Log.traceln("Interval");
        blink();
        previousMillis = currentMillis;
        if (fsConfigFile.get("hostname", s)) {
            Log.noticeln("hostname: %s", s);
        };
        if (fsConfigFile.get("port", p)) {
            Log.noticeln("port: %d", p);
        }
        while (true) delay(delayLong);
    }
}
