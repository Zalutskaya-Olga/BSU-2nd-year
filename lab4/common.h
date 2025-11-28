#ifndef COMMON_H
#define COMMON_H

#include <string>

const int MAX_LEN = 20;
const int MAX_FILE = 100;

struct Msg {
    char text[MAX_LEN + 1];
    bool is_valid;  
};

struct SharedData {
    Msg messages[MAX_FILE];  
    int read_index;          
    int write_index;         
    int count;               
    int max_messages;        
    int sender_count;        
    int ready_senders;      
    bool receiver_ready;     
    bool shutdown;           
};

#endif
