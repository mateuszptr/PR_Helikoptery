#ifndef DEF_H
#define DEF_H

#include <mpi.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

//Typy

class timestamp {
public:
    int first;
    int second;
    
    timestamp(){
        
    }

    timestamp(int i, int j) {
        first=i;
        second=j;
    }
    
    bool operator>(timestamp t) {
        if(this->first > t.first) return true;
        if(this->first == t.first && this->second > t.second) return true;
        return false;
    }
    
    bool operator<(timestamp t) {
        if(this->first < t.first) return true;
        if(this->first == t.first && this->second < t.second) return true;
        return false;
    }
};

enum State {
    UNINTERESTED,
    INTERESTED,
    BUSY
};

enum Message {
    REQ_START,
    REQ_HANGAR,
    REL_START,
    REL_HANGAR
};

typedef struct msg_s {
    int rank;
    int msg;
    int ts_1;
    int ts_2;
} msg_s;

//Stałe

const int H = 10;
const int P = 5;
const int S = 4;
const int IT = 100;

//Czynności

void lot();
void ladowanie();
void postoj();
void start();

//Require

void req_start();
void req_hangar();

//Release

void rel_start();
void rel_hangar();

//MPI wrap
void init(int argc, char **argv);
void init_mutex();
void init_struct();
void finalize();

//Send an Recv wrap
void send(int receiver, Message msg, timestamp ts);
void sendAll(Message msg, timestamp ts);
msg_s recv();

//Monitor thread
void* monitor(void* arg);

//Monitor actions
void on_req_hangar(int receiver, Message msg, timestamp ts_i);
void on_req_start(int receiver, Message msg, timestamp ts_i);
void on_rel_hangar(int receiver, Message msg, timestamp ts_i);
void on_rel_start(int receiver, Message msg, timestamp ts_i);

#endif // DEF_H

