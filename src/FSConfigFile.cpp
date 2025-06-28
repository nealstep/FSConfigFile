#include "FSConfigFile.h"

#include <ArduinoLog.h>
#include <LittleFS.h>

#if not defined(ESP8266) and not defined(ESP32)
#error "Not supported"
#endif  // ! ESP8266 and ! ESP32

const char FSConfigFile::defaultFilename[nameLength] = "/config.json";

bool safe_copy(char *dest, const char *src, size_t size) {
    if (strlen(src) < size) {
        strlcpy(dest, src, size);
    } else {
        return false;
    }
    return true;
}

FSConfigFile::FSConfigFile(const char *filename) {
    Log.traceln("FSConfigFile init");
    if (!safe_copy(fname, filename, nameLength)) {
        Log.errorln("Filename copy failed");
    }
}

bool FSConfigFile::exists(void) {
    bool ret;

    Log.traceln("FSConfigFile::exists");
    LittleFS.begin();
    ret = LittleFS.exists(fname);
    LittleFS.end();
    return ret;
}

bool FSConfigFile::read(void) {
    bool ret = true;
    JsonDocument jDoc;

    Log.traceln("FSConfigFile::read");
    LittleFS.begin();
    File configFile = LittleFS.open(fname, "r");
    if (configFile) {
        auto err = deserializeJson(jDoc, configFile);
        if (err) {
            Log.errorln("Deserialize failed: %s", err.c_str());
            ret = false;
        }
        configFile.close();
    } else {
        Log.errorln("Failed to open config file");
        ret = false;
    }
    LittleFS.end();
    if (!ret) return false;
    for (uint8_t i = 0; i < configCount; i++) {
        if (!jDoc[config[i].name].is<JsonVariant>()) {
            Log.warning("Key not found: %s", config[i].name);
            ret = false;
        }
        switch (config[i].type) {
            case CHARS:
                if (jDoc[config[i].name].is<const char *>()) {
                    if (!safe_copy(config[i].datum.chars,
                                       jDoc[config[i].name], config[i].size)) {
                        Log.errorln("String copy failed for %s",
                                    config[i].name);
                        ret = false;
                    }
                } else {
                    Log.warning("Bad type not chars: %s", config[i].name);
                    ret = false;
                }
                break;
            case U16:
                if (jDoc[config[i].name].is<uint16_t>())
                    config[i].datum.u16 = jDoc[config[i].name];
                else {
                    Log.warning("Bad type not u16: %s", config[i].name);
                    ret = false;
                }
                break;
            case I16:
                if (jDoc[config[i].name].is<int16_t>())
                    config[i].datum.i16 = jDoc[config[i].name];
                else {
                    Log.warning("Bad type not i16: %s", config[i].name);
                    ret = false;
                }
                break;
            default:
                Log.errorln("Unexpected type for %s: %d", config[i].name,
                            static_cast<uint8_t>(config[i].type));
                ret = false;
        }
    }
    return ret;
}

bool FSConfigFile::write(void) {
    bool ret = true;
    JsonDocument jDoc;

    Log.traceln("FSConfigFile::write");
    for (uint8_t i = 0; i < configCount; i++) {
        switch (config[i].type) {
            case CHARS:
                jDoc[config[i].name] = config[i].datum.chars;
                break;
            case U16:
                jDoc[config[i].name] = config[i].datum.u16;
                break;
            case I16:
                jDoc[config[i].name] = config[i].datum.i16;
                break;
            default:
                Log.errorln("Unexpected type for %s: %d", config[i].name,
                            static_cast<uint8_t>(config[i].type));
                ret = false;
        }
    }
    LittleFS.begin();
    File configFile = LittleFS.open(fname, "w");
    if (configFile) {
        if (serializeJson(jDoc, configFile) == 0) {
            Log.errorln("Serialize failed");
            ret = false;
        }
        configFile.close();
    } else {
        Log.errorln("Write failed");
        ret = false;
    }
    LittleFS.end();
    return ret;
}

const char *FSConfigFile::getName(uint8_t id) {
    if (id < configCount) {
        return config[id].name;
    }
    return nullptr;
}

uint8_t FSConfigFile::find(const char *name, DataTypeCodes type) {
    uint8_t ret = badItem;
    bool found = false;

    Log.traceln("FSConfigFile::find");
    for (uint8_t i = 0; i < configCount; i++) {
        if (strncmp(config[i].name, name, nameLength) == 0) {
            found = true;
            if (type != ANY) {
                if (config[i].type != type) {
                    Log.errorln("Item %s wrong type: %d not %d", name,
                                static_cast<uint8_t>(config[i].type),
                                static_cast<uint8_t>(type));
                    break;
                }
            }
            ret = i;
            break;
        }
    }
    if ((!found) && (type != ANY)) {
        Log.errorln("Item not found: %s", name);
    }
    return ret;
}

bool FSConfigFile::add(const char *name, DataTypeCodes type, uint8_t size) {
    uint8_t item;

    Log.traceln("FSConfigFile::add");
    if (strlen(name) > (nameLength - 1)) {
        Log.warning("Name too long max %d: %s", nameLength - 1, name);
        return false;
    }
    item = find(name, ANY);
    if (item != badItem) {
        Log.warning("Name already exists: %s", name);
        return false;
    }
    if (configCount >= configItems) {
        Log.errorln("Config items full while adding %s", name);
        return false;
    }
    item = configCount++;
    if (!safe_copy(config[item].name, name, nameLength)) {
        Log.errorln("Copy failed for new item: %s", name);
    }
    config[item].type = type;
    config[item].size = size;
    if (type == CHARS) {
        config[item].datum.chars = new char[size];
        if (!config[item].datum.chars) {
            Log.errorln("New failed for new item: %s", name);
            return false;
        }
    }
    return true;
}

FSConfigFile::DataTypeCodes FSConfigFile::getType(const char *name) {
    uint8_t item;

    Log.traceln("FSConfigFile::getType");
    item = find(name, ANY);
    if (item == badItem) {
        Log.error("Item not found");
    }
    return config[item].type;
}

bool FSConfigFile::get(const char *name, const char *&datum) {
    uint8_t item;

    Log.traceln("FSConfigFile::get chars");
    item = find(name, CHARS);
    if (item == badItem) return false;
    datum = config[item].datum.chars;
    return true;
}

bool FSConfigFile::get(const char *name, uint16_t &datum) {
    uint8_t item;

    Log.traceln("FSConfigFile::get u16");
    item = find(name, U16);
    if (item == badItem) return false;
    datum = config[item].datum.u16;
    return true;
}

bool FSConfigFile::get(const char *name, int16_t &datum) {
    uint8_t item;

    Log.traceln("FSConfigFile::get i16");
    item = find(name, I16);
    if (item == badItem) return false;
    datum = config[item].datum.i16;
    return true;
}

bool FSConfigFile::set(const char *name, const char *datum) {
    uint8_t item;

    Log.traceln("FSConfigFile::set chars");
    item = find(name, CHARS);
    if (item == badItem) return false;
    if (!safe_copy(config[item].datum.chars, datum, config[item].size)) {
        Log.errorln("Setting string for %s failed", name);
        return false;
    }
    return true;
}

bool FSConfigFile::set(const char *name, const uint16_t &datum) {
    uint8_t item;

    Log.traceln("FSConfigFile::set u16");
    item = find(name, U16);
    if (item == badItem) return false;
    config[item].datum.u16 = datum;
    return true;
}

bool FSConfigFile::set(const char *name, const int16_t &datum) {
    uint8_t item;

    Log.traceln("FSConfigFile::set i16");
    item = find(name, I16);
    if (item == badItem) return false;
    config[item].datum.i16 = datum;
    return true;
}
