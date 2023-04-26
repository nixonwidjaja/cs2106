#include <semaphore.h>
#include <stddef.h>
#include "packer.h"
int n;
sem_t wait_other[4];
sem_t accept_ball[4];
sem_t mutex[4];
int arr[4][70];
int now[4];

void packer_init(int balls_per_pack) {
    n = balls_per_pack;
    for (int i=0;i<4;i++) {
        sem_init(&mutex[i], 0, 1);
        sem_init(&wait_other[i], 0, 0);
        sem_init(&accept_ball[i], 0, n);
        now[i] = 0;
    }
    for (int i=0;i<4;i++) {
        for (int j=0;j<70;j++) {
            arr[i][j] = -1;
        }
    }
}

void packer_destroy(void) {
    for (int i=0;i<n;i++) {
        sem_destroy(&mutex[i]);
        sem_destroy(&wait_other[i]);
        sem_destroy(&accept_ball[i]);
    }
}

void pack_ball(int color, int id, int *other_ids) {
    sem_wait(&accept_ball[color]);

    sem_wait(&mutex[color]);
    arr[color][now[color]] = id;
    now[color]++;
    if (now[color] == n) {
        for (int i=0;i<n;i++) {
            sem_post(&wait_other[color]);
        }
    }
    sem_post(&mutex[color]);
    sem_wait(&wait_other[color]);

    sem_wait(&mutex[color]);
    now[color]--;
    int k = 0;
    for (int i=0;i<n;i++) {
        if (arr[color][i] != id) {
            other_ids[k] = arr[color][i];
            k++;
        }
    }
    if (now[color] == 0) {
        for (int i=0;i<n;i++) {
            arr[color][i] = -1;
        }
        for (int i=0;i<n;i++) {
            sem_post(&accept_ball[color]);
        }
    }
    sem_post(&mutex[color]);
}
