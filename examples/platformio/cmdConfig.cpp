#include "cmdConfig.h"

#include "FSConfigFile.h"

extern FSConfigFile fsConfigFile;

void cmdGetConfigNames(void) {
    uint8_t id = 0;
    const char *name;

    while (true) {
        name = fsConfigFile.getName(id++);
        if (!name) break;
        Log.noticeln("config name %d: %s", id, name);
    }
}

bool cmdGetConfig(const char *name) {
    bool ret = true;

    auto type = fsConfigFile.getType(name);
    switch (type) {
        case (fsConfigFile.CHARS):
            const char *chars;
            fsConfigFile.get(name, chars);
            Log.noticeln("get %s: %s", name, chars);
            break;
        case (fsConfigFile.U16):
            uint16_t u16;
            fsConfigFile.get(name, u16);
            Log.noticeln("get %s: %u", name, u16);
            break;
        case (fsConfigFile.I16):
            int16_t i16;
            fsConfigFile.get(name, i16);
            Log.noticeln("get %s: %i", name, i16);
            break;
        default:
            ret = false;
            Log.error("Unknown type for %s", name);
    }
    return ret;
}

bool cmdSetConfig(const char *name, const char *value) {
    bool ret = true;
    int32_t num;

    auto type = fsConfigFile.getType(name);
    Log.traceln("cmdSetConfig");
    Log.verboseln("Name: %s", name);
    Log.verboseln("Values: %s", value);
    Log.verboseln("Type: %u", static_cast<uint8_t>(type));
    switch (type) {
        case FSConfigFile::CHARS:
            fsConfigFile.set(name, value);
            break;
        case FSConfigFile::U16:
            uint16_t u16;
            num = atoi(value);
            u16 = static_cast<uint16_t>(num);
            if (num != u16) {
                Log.error("Conversion error: %d != %u", num, u16);
            }
            fsConfigFile.set(name, u16);
            break;
        case FSConfigFile::I16:
            int16_t i16;
            num = atoi(value);
            i16 = static_cast<int16_t>(num);
            if (num != i16) {
                Log.error("Conversion error: %d != %d", num, u16);
            }
            fsConfigFile.set(name, i16);
            break;
        default:
            Log.errorln("Unexpected type for %s: %d", name,
                        static_cast<uint8_t>(type));
            ret = false;
    }
    return ret;
}

bool cmdConfig(const char *cmd, uint8_t len) {
    bool ret = true;

    Log.traceln("cmdConfig");
    if (len > 0) {
        switch (cmd[0]) {
            case 'l':
                Log.noticeln("Running Cmd: C%s", cmd);
                cmdGetConfigNames();
                break;
            case 'g':
                if (len < 3) {
                    ret = false;
                    Log.warning("Command not long enough: C%s", cmd);
                } else {
                    Log.noticeln("Running Cmd: C%s", cmd);
                    ret = cmdGetConfig(&cmd[2]);
                }
                break;
            case 's':
                if (len < 5) {
                    ret = false;
                    Log.warning("Command not long enough: C%s", cmd);
                } else {
                    Log.noticeln("Running Cmd: C%s", cmd);
                    auto value = splitString(&cmd[2]);
                    cmdSetConfig(&cmd[2], value);
                }
                break;
            case 'W':
                ret = fsConfigFile.write();
                if (ret)
                    Log.noticeln("Write succeeded");
                else
                    Log.errorln("Write failed");
                break;
            case 'R':
                ret = fsConfigFile.read();
                if (ret)
                    Log.noticeln("Read succeeded");
                else
                    Log.errorln("Read failed");
                break;
            default:
                Log.warningln("Unknown config cmd: C%s", cmd);
        }
    } else {
        Log.errorln("No config command");
    }
    ret = false;
    return ret;
}
