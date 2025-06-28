#ifndef STREAM_CMD_H
#define STREAM_CMD_H

#include <Arduino.h>

class StreamCmd {
   private:
    static const uint8_t _sizeCmd = 64;

   public:
    StreamCmd(void);
    void setStream(Stream &stream);
    bool loop(void);
    inline char *get(void) { return _cmd; }

   private:
    Stream *_stream;
    uint8_t _cmd_ind;
    char _cmd[_sizeCmd];
    bool rdy;
};

#endif  // STREAM_CMD_H
