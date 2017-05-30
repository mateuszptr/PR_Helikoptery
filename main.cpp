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

	const int nitems = 2;
	int blocklengths[2] = {1, 1};
	MPI_Datatype types[2] = {MPI_INT, MPI_INT};
	MPI_Aint offsets[2];
	
	MPI_Datatype mpi_msg_type;
	MPI_Datatype mpi_timestamp_type;	

	offsets[0] = offsetof(timestamp, timestamp.first);
	offsets[1] = offsetof(timestamp, timestamp.second);

	MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_timestamp_type);
	MPI_Type_commit(&mpi_timestamp_type);
	
	MPI_Datatype types2[2] = {MPI_INT, mpi_timestamp_type};
	MPI_Aint offsets2[2];
	offsets2[0] = offsetof(msg_s, message);
	offsets2[1] = offsetof(msg_s, ts);

	MPI_Type_create_struct(nitems, blocklengths, offsets2, types2, &mpi_msg_type);
	MPI_Type_commit(&mpi_msg_type);
	
    for (int i = 0; i < IT; i++) {
        lot();
        ladowanie();
        postoj();
        start();
    }

	MPI_Type_free(&mpi_timestamp_type);
	MPI_Type_free(&mpi_msg_type);
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

void sendAll(Message msg, timestamp ts) {
	struct msg_s msg1;
	msg1.message = (int)Message.msg;
	msg1.ts = ts;
	MPI_Bcast($msg1, 1, mpi_msg_type, 0, MPI_COMM_WORLD);
}

void send(int i, Message msg, timestamp ts) {
	struct msg_s msg1;
	msg1.message = (int)Message.msg;
	msg1.ts = ts;
	MPI_Send(&msg1, 1, mpi_msg_type, i, 0, MPI_COMM_WORLD);
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