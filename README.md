# FSConfig

Manage a config file in littlefs on ESP32 or ESP8266

## Commands

* Cl - list config names
* Cg NAME - get value of config item NAME
* Cs NAME VALUE - set config item NAME to VALUE
* CW - Write config to littleFS
* CR - Read config from littleFS

## Notes

A lot of testing code is included to allow for good testing. You only need the two FsConfigFile files (.h, .cpp) for it to work
