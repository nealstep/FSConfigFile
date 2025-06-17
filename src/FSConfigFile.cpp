#include "FSConfigFile.h"

#include "FS.h"
#include <LittleFS.h>

#if not defined(ESP8266) and not defined(ESP32)
#error "Not supported"
#endif  // ! ESP8266 and ! ESP32

FSConfigFile::FSConfigFile(const char *fname) {
    filename = fname;
    if (!LittleFS.begin()) {
        LittleFS.format();
        fs_ok = false;
    } else {
        fs_ok = true;
    }
}

bool FSConfigFile::read(void) {
    if (!LittleFS.exists(filename)) {
        if (!create()) {
            return false;
        }
    }
    File configFile = LittleFS.open(filename, "r");
    if (!configFile) {
        return false;
    }
    auto error = deserializeJson(config, configFile);
    configFile.close();
    if (error) {
        return false;
    }
    return true;
}

bool FSConfigFile::create(void) {
    File configFile = LittleFS.open(filename, "w");
    if (!configFile) {
        return false;
    }
    configFile.write("");
    configFile.close();
    return true;
}

bool FSConfigFile::write(void) {
    File configFile = LittleFS.open(filename, "w");
    if (!configFile) {
        return false;
    }
    serializeJson(config, configFile);
    configFile.close();
    return true;
}

