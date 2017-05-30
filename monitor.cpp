#include "def.h"

extern timestamp ts, ts_r;
extern State state_hangar;
extern State state_start;

extern int RD_H[];
extern int RD_S[];

extern pthread_mutex_t mutex_state_hangar;
extern pthread_mutex_t mutex_state_start;
extern pthread_mutex_t mutex_ts;
extern pthread_mutex_t mutex_ts_r;
extern pthread_mutex_t mutex_rd_h;
extern pthread_mutex_t mutex_rd_s;

extern sem_t sem_hangar;
extern sem_t sem_start;

int recv_h=0;
int recv_s=0;

void* monitor(void* arg) {

    timestamp ts_r;
    Message msg;
    int p;

    while (1) {
        recv(&p, &msg, &ts_r);
        switch (msg) {
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
    pthread_mutex_lock(&mutex_state_hangar);
    pthread_mutex_lock(&mutex_ts);
    pthread_mutex_lock(&mutex_ts_r);
    pthread_mutex_lock(&mutex_rd_h);

    if (state_hangar != BUSY && (state_hangar == UNINTERESTED || ts_i < ts_r)) {
        send(receiver, REL_HANGAR, ts);
    } else {
        RD_H[receiver] = 1;
    }

    pthread_mutex_unlock(&mutex_rd_h);
    pthread_mutex_unlock(&mutex_ts_r);
    pthread_mutex_unlock(&mutex_ts);
    pthread_mutex_unlock(&mutex_state_hangar);
}

void on_req_start(int receiver, Message msg, timestamp ts_i) {
    pthread_mutex_lock(&mutex_state_start);
    pthread_mutex_lock(&mutex_ts);
    pthread_mutex_lock(&mutex_ts_r);
    pthread_mutex_lock(&mutex_rd_s);

    if (state_start != BUSY && (state_start == UNINTERESTED || ts_i < ts_r)) {
        send(receiver, REL_START, ts);
    } else {
        RD_S[receiver] = 1;
    }

    pthread_mutex_unlock(&mutex_rd_s);
    pthread_mutex_unlock(&mutex_ts_r);
    pthread_mutex_unlock(&mutex_ts);
    pthread_mutex_unlock(&mutex_state_start);
}

void on_rel_hangar(int receiver, Message msg, timestamp ts_i) {
    pthread_mutex_lock(&mutex_ts_r);
    int semval;
    sem_getvalue(&sem_hangar, &semval);
    if (ts_r < ts_i && semval < 0) {
        recv_h++;
        if(recv_h==H) {
            recv_h=0;
            sem_post(&sem_hangar);
        }
    }
    pthread_mutex_unlock(&mutex_ts_r);
}

void on_rel_start(int receiver, Message msg, timestamp ts_i) {
    pthread_mutex_lock(&mutex_ts_r);
    int semval;
    sem_getvalue(&sem_start, &semval);
    if (ts_r < ts_i && semval < 0) {
        recv_s++;
        if(recv_s==H) {
            recv_s=0;
            sem_post(&sem_start);
        }
    }
    pthread_mutex_unlock(&mutex_ts_r);
}