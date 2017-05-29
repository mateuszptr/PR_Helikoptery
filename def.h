#ifndef DEF_H
#define DEF_H

#include <mpi.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

//Typy

typedef std::pair<int, int> timestamp;

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

//Stałe

const int H = 10;
const int P = 8;
const int S = 4;
const int IT = 1;

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
void finalize();

//Send an Recv wrap
void send(int receiver, Message msg, timestamp ts);
void sendAll(Message msg, timestamp ts);
void recv(int* sender, Message* msg, timestamp* ts);

//Monitor thread
void* monitor(void* arg);

//Monitor actions
void on_req_hangar(int receiver, Message msg, timestamp ts_i);
void on_req_start(int receiver, Message msg, timestamp ts_i);
void on_rel_hangar(int receiver, Message msg, timestamp ts_i);
void on_rel_start(int receiver, Message msg, timestamp ts_i);

#endif // DEF_H

