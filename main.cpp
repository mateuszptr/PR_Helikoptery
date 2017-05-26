#include "def.h"

timestamp ts;

int RD_H[H];
int RD_S[H];

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
}

void finalize() {
    printf("h%d: Kończę!\n", rank);
    MPI_Finalize();
}