#ifndef DEF_H
#define DEF_H

#include <mpi.h>
#include <stdio.h>

//Typy

typedef std::pair<int, int> timestamp;

enum stan {
    LOT,
    POSTOJ,
    START,
    OTHER
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

#endif	// DEF_H

