#include "FSConfigFile.h"

#include <LittleFS.h>

#include "FS.h"

#if not defined(ESP8266) and not defined(ESP32)
#error "Not supported"
#endif  // ! ESP8266 and ! ESP32

const char FSConfigFile::defaultFilename[nameLength] = "/config.json";

FSConfigFile::FSConfigFile(const char *filename) { 
    strlcpy(fname, filename, nameLength);
}

bool FSConfigFile::exists(void) {
    bool ret;

    LittleFS.begin();
    ret = LittleFS.exists(fname);
    LittleFS.end();
    return ret;
}

bool FSConfigFile::read(void) {
    JsonDocument jsonDocument;

    File configFile = LittleFS.open(fname, "r");
    if (!configFile) {
        return false;
    }
    dError = deserializeJson(jsonDocument, configFile);
    configFile.close();
    if (dError) {
        return false;
    }
    for (uint8_t i = 0; i < configCount; i++) {
        switch (config[i].type) {
            case CHARS:
                strlcpy(config[i].datum.chars, jsonDocument[config[i].name], config[i].size);
                break;
            case U16:
                config[i].datum.u16 = jsonDocument[config[i].name];
                break;
            case I16:
                config[i].datum.i16 = jsonDocument[config[i].name];
                break;
            default:
                return false;
        }
    }
    return true;
}

bool FSConfigFile::write(void) {
    bool ret = true;
    JsonDocument jsonDocument;
    for (uint8_t i = 0; i < configCount; i++) {
        switch (config[i].type) {
            case CHARS:
                jsonDocument[config[i].name] = config[i].datum.chars;
                break;
            case U16:
                jsonDocument[config[i].name] = config[i].datum.u16;
                break;
            case I16:
                jsonDocument[config[i].name] = config[i].datum.i16;
                break;
            default:
                return false;
        }
    }
    File configFile = LittleFS.open(fname, "w");
    if (!configFile) {
        return false;
    }
    if (serializeJson(jsonDocument, configFile) == 0) ret = false;
    configFile.close();
    return ret;
}

uint8_t FSConfigFile::find(const char *name, DataTypeCodes type) {
    uint8_t ret = badItem;

    for (uint8_t i = 0; i < configCount; i++) {
        if (strncmp(config[i].name, name, nameLength) == 0) {
            if (type != ANY) {
                if (config[i].type != type) {
                    break;
                }
            }
            ret = i;
            break;
        }
    }
    return ret;
}

bool FSConfigFile::add(const char *name, DataTypeCodes type, uint8_t size) {
    uint8_t item;

    if (strlen(name) > (nameLength-1)) {
        return false;
    }
    item = find(name, ANY);
    if (item != badItem) {
        return false;
    }
    if (configCount >= configItems) {
        return false;
    }
    item = configCount++;
    strlcpy(config[item].name, name, nameLength);
    config[item].type = type;
    config[item].size = size;
    if (type == CHARS) {
        config[item].datum.chars = new char[size];
    }
    return true;
}

bool FSConfigFile::get(const char *name, const char *&datum) {
    uint8_t item;

    item = find(name, CHARS);
    if (item == badItem) return false;
    datum = config[item].datum.chars;
    return true;
}

bool FSConfigFile::get(const char *name, uint16_t &datum) {
    uint8_t item;

    item = find(name, U16);
    if (item == badItem) return false;
    datum = config[item].datum.u16;
    return true;
}

bool FSConfigFile::get(const char *name, int16_t &datum) {
    uint8_t item;

    item = find(name, I16);
    if (item == badItem) return false;
    datum = config[item].datum.i16;
    return true;
}

bool FSConfigFile::set(const char *name, const char *datum) {
    uint8_t item;

    item = find(name, CHARS);
    if (item == badItem) return false;
    strlcpy(config[item].datum.chars, datum, config[item].size);
    return true;
}

bool FSConfigFile::set(const char *name, const uint16_t &datum) {
    uint8_t item;

    item = find(name, U16);
    if (item == badItem) return false;
    config[item].datum.u16 = datum;
    return true;
}

bool FSConfigFile::set(const char *name, const int16_t &datum) {
    uint8_t item;

    item = find(name, I16);
    if (item == badItem) return false;
    config[item].datum.i16 = datum;
    return true;
}
