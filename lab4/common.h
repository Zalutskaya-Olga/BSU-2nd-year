#ifndef COMMON_H
#define COMMON_H

#include <string>

const int MAX_LEN = 20;
const int MAX_FILE = 100;

struct Msg {
    char text[MAX_LEN + 1];
    bool empty;
};

struct SharedData {
    Msg msg;
    bool sender_ready;
    bool receiver_ready;
};

#endif