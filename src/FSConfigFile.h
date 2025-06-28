#ifndef FSCONFIGFILE_H
#define FSCONFIGFILE_H

#include <Arduino.h>
#include <ArduinoJson.h>

class FSConfigFile {
   public:
    enum DataTypeCodes { ANY, CHARS, U16, I16 };

   private:
    static const uint8_t nameLength = 16;
    static const uint8_t configItems = 10;
    static const uint8_t badItem = 255;
    static const char defaultFilename[nameLength];
    union DataTypes {
        char *chars;
        uint16_t u16;
        int16_t i16;
    };
    struct DataPair {
        char name[nameLength];
        DataTypeCodes type;
        uint8_t size;
        DataTypes datum;
    };

   public:
    FSConfigFile(const char *filename = defaultFilename);
    bool exists(void);
    bool read(void);
    bool write(void);
    const char *getName(uint8_t id);
    bool add(const char *name, DataTypeCodes type, uint8_t size = 0);
    DataTypeCodes getType(const char *name);
    bool get(const char *name, const char *&datum);
    bool get(const char *name, uint16_t &datum);
    bool get(const char *name, int16_t &datum);
    bool set(const char *name, const char *datum);
    bool set(const char *name, const uint16_t &datum);
    bool set(const char *name, const int16_t &datum);

   private:
    char fname[nameLength];
    DataPair config[configItems];
    uint8_t configCount = 0;
    DeserializationError dError;

    uint8_t find(const char *name, DataTypeCodes type);
};

#endif  // FSCONFIGFILE_H