#ifndef FSCONFIGFILE_H
#define FSCONFIGFILE_H

#include <Arduino.h>
#include <ArduinoJson.h>

#define DEFAULT_FILENAME "/config.json"

class FSConfigFile {
    public:
        bool fs_ok = false;
        JsonDocument config;
        FSConfigFile(const char *fname=DEFAULT_FILENAME);
        bool read(void);
        bool create(void);
        bool write(void);
    private:
        String filename;
};

#endif  // FSCONFIGFILE_H