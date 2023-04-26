#include "packer.h"
#include <semaphore.h>
// You can declare global variables here
sem_t wait_other[4];
sem_t accept_ball[4];
sem_t mutex[4];
int arr[4][2], now[4];

void packer_init(void) {
    // Write initialization code here (called once at the start of the program).
    for (int i=0;i<4;i++) {
        sem_init(&wait_other[i], 0, 0);
        sem_init(&accept_ball[i], 0, 2);
        sem_init(&mutex[i], 0, 1);
        now[i] = 0;
    }
    for (int i=0;i<4;i++) {
        for (int j=0;j<2;j++) {
            arr[i][j] = -1;
        }
    }
}

void packer_destroy(void) {
    // Write deinitialization code here (called once at the end of the program).
    for (int i=0;i<4;i++) {
        sem_destroy(&wait_other[i]);
        sem_destroy(&accept_ball[i]);
        sem_destroy(&mutex[i]);
    }
}

int pack_ball(int color, int id) {
    // Write your code here.
    int ans, idx;
    sem_wait(&accept_ball[color]);
    
    sem_wait(&mutex[color]);
    idx = now[color];
    arr[color][now[color]] = id;
    now[color]++;
    if (now[color] == 2) {
        sem_post(&wait_other[color]);
        sem_post(&wait_other[color]);
    }
    sem_post(&mutex[color]);
    sem_wait(&wait_other[color]);

    sem_wait(&mutex[color]);
    now[color]--;
    ans = arr[color][1 - idx];
    if (now[color] == 0) {
        arr[color][0] = -1;
        arr[color][1] = -1;
        sem_post(&accept_ball[color]);
        sem_post(&accept_ball[color]);
    }
    sem_post(&mutex[color]);

    return ans;
}