#include "def.h"

timestamp ts, ts_r;
State state_hangar;
State state_start;

pthread_t monitor_handle;

pthread_mutex_t mutex_state_hangar;
pthread_mutex_t mutex_state_start;
pthread_mutex_t mutex_ts;
pthread_mutex_t mutex_ts_r;
pthread_mutex_t mutex_rd_h;
pthread_mutex_t mutex_rd_s;

sem_t sem_hangar;
sem_t sem_start;


pthread_mutexattr_t mutex_state_hangar_attr;
pthread_mutexattr_t mutex_state_start_attr;
pthread_mutexattr_t mutex_ts_attr;
pthread_mutexattr_t mutex_ts_r_attr;
pthread_mutexattr_t mutex_rd_h_attr;
pthread_mutexattr_t mutex_rd_s_attr;

MPI_Datatype mpi_message;

int RD_H[H] = {0};
int RD_S[H] = {0};

int rank, size;
char processor_name[MPI_MAX_PROCESSOR_NAME];
int namelen;

bool finished;

int main(int argc, char **argv) {


    init(argc, argv);

    for (int i = 0; i < IT; i++) {
        lot();
        req_hangar();
        //printf("H%d: %d Rez Hangar\n", rank, i);
        req_start();
        //printf("H%d: Rez Start\n", rank);
        ladowanie();
        rel_start();
        //printf("H%d: Zw Start\n", rank);
        postoj();
        req_start();
        //printf("H%d: Rez Start\n", rank);
        rel_hangar();
        //printf("H%d: Zw Hangar\n", rank);
        start();
        rel_start();
        //printf("H%d: Zw Start\n", rank);
    }

    MPI_Type_free(&mpi_message);
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
    setbuf(stdout, NULL);

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(processor_name, &namelen);

    ts = timestamp(0, rank);
    ts_r = timestamp(-1, rank);
    state_hangar = UNINTERESTED;
    state_start = UNINTERESTED;

    finished = false;

    init_mutex();
    init_struct();

    pthread_create(&monitor_handle, NULL, monitor, NULL);



}

void init_mutex() {
    pthread_mutexattr_init(&mutex_state_hangar_attr);
    pthread_mutex_init(&mutex_state_hangar, &mutex_state_hangar_attr);


    pthread_mutexattr_init(&mutex_state_start_attr);
    pthread_mutex_init(&mutex_state_start, &mutex_state_start_attr);


    pthread_mutexattr_init(&mutex_ts_attr);
    pthread_mutex_init(&mutex_ts, &mutex_ts_attr);

    pthread_mutexattr_init(&mutex_ts_r_attr);
    pthread_mutex_init(&mutex_ts_r, &mutex_ts_r_attr);

    pthread_mutexattr_init(&mutex_rd_h_attr);
    pthread_mutex_init(&mutex_rd_h, &mutex_rd_h_attr);

    pthread_mutexattr_init(&mutex_rd_s_attr);
    pthread_mutex_init(&mutex_rd_s, &mutex_rd_s_attr);

    sem_init(&sem_hangar, 0, 0);
    sem_init(&sem_start, 0, 0);

}

void init_struct() {
    int blocklengths[] = {1, 1, 1, 1};
    MPI_Datatype types[] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Aint offsets[] = {offsetof(msg_s, rank), offsetof(msg_s, msg), offsetof(msg_s, ts_1), offsetof(msg_s, ts_2)};

    MPI_Type_create_struct(4, blocklengths, offsets, types, &mpi_message);
    MPI_Type_commit(&mpi_message);
}

void finalize() {
    printf("h%d: Kończę!\n", rank);

    pthread_cancel(monitor_handle);

    MPI_Finalize();

    pthread_mutex_destroy(&mutex_state_hangar);
    pthread_mutex_destroy(&mutex_state_start);
    pthread_mutex_destroy(&mutex_ts);
    pthread_mutex_destroy(&mutex_ts_r);
    pthread_mutex_destroy(&mutex_rd_h);
    pthread_mutex_destroy(&mutex_rd_s);

    sem_destroy(&sem_hangar);
    sem_destroy(&sem_start);


}

void sendAll(Message msg, timestamp ts_s) {
    msg_s packet;
    packet.rank = rank;
    packet.msg = msg;
    packet.ts_1 = ts_s.first;
    packet.ts_2 = ts_s.second;
    for (int i = 0; i < H; i++) if (i != rank)
            MPI_Send(&packet, 1, mpi_message, i, 0, MPI_COMM_WORLD);
    ts.first++;
}

void send(int i, Message msg, timestamp ts_s) {
    msg_s packet;
    packet.rank = rank;
    packet.msg = msg;
    packet.ts_1 = ts_s.first;
    packet.ts_2 = ts_s.second;
    MPI_Send(&packet, 1, mpi_message, i, 0, MPI_COMM_WORLD);
    ts.first++;
}

void req_hangar() {

    pthread_mutex_lock(&mutex_state_hangar);
    state_hangar = INTERESTED;
    pthread_mutex_unlock(&mutex_state_hangar);

    pthread_mutex_lock(&mutex_ts);
    pthread_mutex_lock(&mutex_ts_r);

    ts_r = ts;
    ts_r.first++;
    sendAll(REQ_HANGAR, ts_r);

    pthread_mutex_unlock(&mutex_ts_r);
    pthread_mutex_unlock(&mutex_ts);

    sem_wait(&sem_hangar);


    pthread_mutex_lock(&mutex_state_hangar);
    state_hangar = BUSY;
    pthread_mutex_unlock(&mutex_state_hangar);
}

void req_start() {
    pthread_mutex_lock(&mutex_state_start);
    state_start = INTERESTED;
    pthread_mutex_unlock(&mutex_state_start);

    pthread_mutex_lock(&mutex_ts);
    pthread_mutex_lock(&mutex_ts_r);

    ts_r = ts;
    ts_r.first++;
    sendAll(REQ_START, ts_r);

    pthread_mutex_unlock(&mutex_ts_r);
    pthread_mutex_unlock(&mutex_ts);

    sem_wait(&sem_start);

    pthread_mutex_lock(&mutex_state_start);
    state_start = BUSY;
    pthread_mutex_unlock(&mutex_state_start);
}

void rel_hangar() {
    pthread_mutex_lock(&mutex_state_hangar);
    state_hangar = UNINTERESTED;
    pthread_mutex_unlock(&mutex_state_hangar);

    pthread_mutex_lock(&mutex_ts);
    pthread_mutex_lock(&mutex_rd_h);
    for (int i = 0; i < H; i++) {
        if (RD_H[i] == 1) {
            timestamp ts_e = ts;
            ts_e.first++;
            send(i, REL_HANGAR, ts_e);
            RD_H[i] = 0;
        }
    }
    pthread_mutex_unlock(&mutex_rd_h);
    pthread_mutex_unlock(&mutex_ts);
}

void rel_start() {
    pthread_mutex_lock(&mutex_state_start);
    state_start = UNINTERESTED;
    pthread_mutex_unlock(&mutex_state_start);

    pthread_mutex_lock(&mutex_ts);
    pthread_mutex_lock(&mutex_rd_s);
    for (int i = 0; i < H; i++) {
        if (RD_S[i] == 1) {
            timestamp ts_e = ts;
            ts_e.first++;
            send(i, REL_START, ts_e);
            RD_S[i] = 0;
        }
    }
    pthread_mutex_unlock(&mutex_rd_s);
    pthread_mutex_unlock(&mutex_ts);
}
