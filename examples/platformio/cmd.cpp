#include "cmd.h"
#include "cmdConfig.h"

bool cmdHandle(const char *cmd) {
    bool ret = true;
    uint8_t len = strlen(cmd);

    Log.traceln("cmdHandle");
    if (len > 0) {
        switch (cmd[0]) {
            case 'C':
                ret = cmdConfig(&(cmd[1]), len - 1);
                break;
            default:
                Log.warningln("Unknown cmd: %s", cmd);
                ret = false;
        }
    } else {
        Log.errorln("No command");
    }
    return ret;
}
