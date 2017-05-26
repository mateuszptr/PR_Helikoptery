#include "def.h"

timestamp ts;
State state_hangar;
State state_start;

int RD_H[H]={0};
int RD_S[H]={0};

int rank, size;
char processor_name[MPI_MAX_PROCESSOR_NAME];
int namelen;

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
    
}

void finalize() {
    printf("h%d: Kończę!\n", rank);
    MPI_Finalize();
}