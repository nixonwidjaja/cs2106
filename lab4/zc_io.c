#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <semaphore.h>
#include "zc_io.h"
#define eprintf(msg, ...)      fprintf(stderr, msg, ##__VA_ARGS__)

// The zc_file struct is analogous to the FILE struct that you get from fopen.
struct zc_file {
    // Insert the fields you need here.
    char* vm;
    size_t offset;
    size_t size;
    int fd;
    int readers;
    sem_t read;
    sem_t mutex;
    /* Some suggested fields :
        - pointer to the virtual memory space
        - offset from the start of the virtual memory
        - total size of the file
        - file descriptor to the opened file
        - mutex for access to the memory space and number of readers
    */
};
/**************
 * Exercise 1 *
 **************/

zc_file* zc_open(const char* path) {
    // To implement
    int fd = open(path, O_CREAT|O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) return NULL;

    zc_file* file;
    struct stat sb;
    if (fstat(fd, &sb) == -1) return NULL;
    size_t size = sb.st_size;
    char* vm = size > 0 ? mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0) : NULL;
    if (vm == MAP_FAILED) return NULL;

    file = malloc(sizeof(*file));
    if (!file) return NULL;

    file->vm = vm;
    file->offset = 0;
    file->size = size;
    file->fd = fd;
    file->readers = 0;
    sem_init(&(file->read), 0, 1);
    sem_init(&(file->mutex), 0, 1);
    return file;
}

int zc_close(zc_file* file) {
    // To implement
    if (msync(file->vm, file->size, MS_SYNC) == -1) return -1;
    if (munmap(file->vm, file->size) == -1) return -1;
    if (close(file->fd) == -1) return -1;
    sem_destroy(&(file->read));
    sem_destroy(&(file->mutex));
    free(file);
    return 0;
}

const char* zc_read_start(zc_file* file, size_t* size) {
    // To implement
    sem_wait(&(file->read));
    file->readers++;
    if (file->readers == 1) {
        sem_wait(&(file->mutex));
    }
    char* ans = file->vm + file->offset;
    if (file->offset + *size > file->size) {
        *size = (file->size - file->offset);
    }
    file->offset += *size;
    sem_post(&(file->read));
    return ans;
}

void zc_read_end(zc_file* file) {
    // To implement
    sem_wait(&(file->read));
    file->readers--;
    if (file->readers == 0) {
        sem_post(&(file->mutex));
    }
    sem_post(&(file->read));
}

char* zc_write_start(zc_file* file, size_t size) {
    // To implement
    sem_wait(&(file->mutex));
    if (file->offset + size > file->size) {
        size_t new_size = file->offset + size;
        ftruncate(file->fd, new_size);
        munmap(file->vm, file->size);
        file->vm = mmap(NULL, new_size, PROT_READ|PROT_WRITE, MAP_SHARED, file->fd, 0);
        file->size = new_size;
    }
    char* ans = file->vm + file->offset;
    file->offset += size;
    return ans;
}

void zc_write_end(zc_file* file) {
    // To implement
    msync(file->vm, file->size, MS_SYNC);
    sem_post(&(file->mutex));
}

/**************
 * Exercise 2 *
 **************/

off_t zc_lseek(zc_file* file, long offset, int whence) {
    // To implement
    sem_wait(&(file->mutex));
    if (whence == SEEK_SET) {
        file->offset = offset;
    } else if (whence == SEEK_CUR) {
        file->offset += offset;
    } else if (whence == SEEK_END) {
        file->offset = file->size + offset;
    } else {
        return -1;
    }
    sem_post(&(file->mutex));
    return file->offset;
}

/**************
 * Exercise 3 *
 **************/

int zc_copyfile(const char* source, const char* dest) {
    // To implement
    zc_file* a = zc_open(source);
    zc_file* b = zc_open(dest);
    zc_write_start(b, a->size);
    if (ftruncate(b->fd, a->size) == -1) {
        return -1;
    }
    b->vm = memcpy(b->vm, a->vm, a->size);
    b->size = a->size;
    zc_write_end(b);
    zc_close(a);
    zc_close(b);
    return 0;
}

/**************
 * Bonus Exercise *
 **************/

const char* zc_read_offset(zc_file* file, size_t* size, long offset) {
    // To implement
    return NULL;
}

char* zc_write_offset(zc_file* file, size_t size, long offset) {
    // To implement
    return NULL;
}