#include "def.h"

extern timestamp ts, ts_r;
extern State state_hangar;
extern State state_start;

void* monitor(void* arg) {
    
    timestamp ts_r;
    Message msg;
    int p;
    
    while(1) {
        recv(&p, &msg, &ts_r);
        switch(msg) {
            case REQ_HANGAR:
                on_req_hangar(p, msg, ts_r);
                break;
            case REQ_START:
                on_req_start(p, msg, ts_r);
                break;
            case REL_HANGAR:
                on_rel_hangar(p, msg, ts_r);
                break;
            case REL_START:
                on_rel_start(p, msg, ts_r);
                break;
        }
    }
}

void on_req_hangar(int receiver, Message msg, timestamp ts_i) {
    if(state_hangar != BUSY && (state_hangar == UNINTERESTED || ts_i < ts_r)) {
        send(receiver, REL_HANGAR, ts);
    } else {
        RD_H[receiver] = 1;
    }
}

void on_req_start(int receiver, Message msg, timestamp ts_i) {
    if(state_start != BUSY && (state_start == UNINTERESTED || ts_i < ts_r)) {
        send(receiver, REL_START, ts);
    } else {
        RD_S[receiver] = 1;
    }
}

void on_rel_hangar(int receiver, Message msg, timestamp ts_i) {
    if(ts_r < ts_i) {
        //recv_h++
    }
}

void on_rel_start(int receiver, Message msg, timestamp ts_i) {
    if(ts_r < ts_i) {
        //recv_s++
    }
}