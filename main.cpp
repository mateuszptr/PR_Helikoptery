#include "def.h"

timestamp ts, ts_r;
State state_hangar;
State state_start;

int RD_H[H]={0};
int RD_S[H]={0};

int rank, size;
char processor_name[MPI_MAX_PROCESSOR_NAME];
int namelen;

bool finished;

int main(int argc, char **argv) {

    init(argc, argv);

    for (int i = 0; i < IT; i++) {
        lot();
        ladowanie();
        postoj();
        start();
    }

    finalize();
}

//Czynności

void lot() {
    printf("H%d: Lecę\n", rank);
}

void ladowanie() {
    printf("H%d: Ląduję\n", rank);
}

void postoj() {
    printf("H%d: Stoję w hangarze\n", rank);
}

void start() {
    printf("H%d: Startuję\n", rank);
}

//MPI wrap

void init(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(processor_name, &namelen);
    
    ts = timestamp(0, rank);
    state_hangar = UNINTERESTED;
    state_start = UNINTERESTED;
    
    finished = false;
    
}

void finalize() {
    printf("h%d: Kończę!\n", rank);
    MPI_Finalize();
}

void req_hangar() {
    ts_r = ts;
    sendAll(REQ_HANGAR, ts_r);
    //recv_h
    //wait
}

void req_start() {
    ts_r = ts;
    sendAll(REQ_START, ts_r);
    //recv_s
    //wait
}

void rel_hangar() {
    state_hangar = UNINTERESTED;
    for(int i=0;i<H;i++) {
        if(RD_H[i]==1) {
            send(i, REL_HANGAR, ts);
            RD_H[i]=0;
        }
    }
}

void rel_start() {
    state_start = UNINTERESTED;
    for(int i=0;i<H;i++) {
        if(RD_S[i]==1) {
            send(i, REL_START, ts);
            RD_S[i]=0;
        }
    }
}