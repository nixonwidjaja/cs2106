#include "packer.h"
#include <semaphore.h>
#include <sys/queue.h>
// You can declare global variables here
int queue[5][1000] = {0};
int queue_head_tail[5][2] = {0};
int semaphore_head_tail[5][2] = {0};
sem_t ball_semaphore[5][1000];
sem_t insert_mutex[5];
sem_t extract_mutex[5];
sem_t num_balls[5];

void packer_init(void) {
    for (int i=0;i<5;i++) {
        sem_init(&insert_mutex[i], 0, 1);
        sem_init(&extract_mutex[i], 0, 1);
        sem_init(&num_balls[i], 0, 0);
        for (int j=0;j<1000;j++) {
            sem_init(&ball_semaphore[i][j], 0, 0);
        }
    }
}

void packer_destroy(void) {
    for (int i=0;i<5;i++) {
        sem_destroy(&insert_mutex[i]);
        sem_destroy(&extract_mutex[i]);
        sem_destroy(&num_balls[i]);
        for (int j=0;j<1000;j++) {
            sem_destroy(&ball_semaphore[i][j]);
        }
    }
}

int pack_ball(int colour, int id) {
    int ans, curr;

    if (colour == 1) {
        sem_wait(&insert_mutex[1]);
        curr = queue_head_tail[1][1];
        queue[1][queue_head_tail[1][1]++] = id;
        sem_post(&ball_semaphore[2][semaphore_head_tail[2][1]++]);
        sem_post(&insert_mutex[1]);

        sem_wait(&ball_semaphore[1][curr]); 
        sem_wait(&extract_mutex[2]);
        ans = queue[2][queue_head_tail[2][0]++];
        sem_post(&extract_mutex[2]);
    } else if (colour == 2) {
        sem_wait(&insert_mutex[2]);
        curr = queue_head_tail[2][1];
        queue[2][queue_head_tail[2][1]++] = id;
        sem_post(&ball_semaphore[1][semaphore_head_tail[1][1]++]);
        sem_post(&insert_mutex[2]);

        sem_wait(&ball_semaphore[2][curr]); 
        sem_wait(&extract_mutex[1]);
        ans = queue[1][queue_head_tail[1][0]++];
        sem_post(&extract_mutex[1]);
    } else if (colour == 3) {
        sem_wait(&insert_mutex[3]);
        curr = queue_head_tail[3][1];
        queue[3][queue_head_tail[3][1]++] = id;
        sem_post(&ball_semaphore[4][semaphore_head_tail[4][1]++]);
        sem_post(&insert_mutex[3]);

        sem_wait(&ball_semaphore[3][curr]); 
        sem_wait(&extract_mutex[4]);
        ans = queue[4][queue_head_tail[4][0]++];
        sem_post(&extract_mutex[4]);
    } else {
        sem_wait(&insert_mutex[4]);
        curr = queue_head_tail[4][1];
        queue[4][queue_head_tail[4][1]++] = id;
        sem_post(&ball_semaphore[3][semaphore_head_tail[3][1]++]);
        sem_post(&insert_mutex[4]);

        sem_wait(&ball_semaphore[4][curr]); 
        sem_wait(&extract_mutex[3]);
        ans = queue[3][queue_head_tail[3][0]++];
        sem_post(&extract_mutex[3]);
    }

    return ans;
}