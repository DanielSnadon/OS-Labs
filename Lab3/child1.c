#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>

#define SHM_SIZE 4096

typedef struct {
    uint32_t length;
    char data[SHM_SIZE - sizeof(uint32_t)];
} shmStruct;

void work(char* inBuf, uint32_t inLen, char* outBuf, uint32_t* outLen) {

    const char banwords[] = "aeiouyAEIOUY";
    size_t counter = 0;

    for (size_t i = 0; i < inLen; i++) {
        if (strchr(banwords, inBuf[i]) == NULL) {
            outBuf[counter++] = inBuf[i];
        }
    }

    *outLen = counter;
}

int main(int argc, char* argv[]) {

    char *filePath = argv[1];
    char *shmName = argv[2];
    char *semFullName = argv[3];
    char *semEmptyName = argv[4];

    int32_t file = open(filePath, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0600);
    if (file == -1) {
		perror("Ошибка: не удалось открыть файл.");
		_exit(EXIT_FAILURE);
	}

    int shm = shm_open(shmName, O_RDWR, 0600);
    if (shm == -1) {
		perror("Ошибка: не удалось открыть SHM.");
        close(file);
		_exit(EXIT_FAILURE);
	}

    shmStruct *shmBuf = (shmStruct *)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
    close(shm);
    if (shmBuf == MAP_FAILED) {
        perror("Ошибка: не удалось отобразить SHM.");
		close(file);
		_exit(EXIT_FAILURE);
    }
    
    sem_t *semFull = sem_open(semFullName, O_RDWR);
    if (semFull == SEM_FAILED) {
		perror("Ошибка: не удалось открыть семафор FULL.");
        munmap(shmBuf, SHM_SIZE);
		close(file);
		_exit(EXIT_FAILURE);
	}

    sem_t *semEmpty = sem_open(semEmptyName, O_RDWR);
    if (semEmpty == SEM_FAILED) {
		perror("Ошибка: не удалось открыть семафор EMPTY.");
        sem_close(semFull);
        munmap(shmBuf, SHM_SIZE);
		close(file);
		_exit(EXIT_FAILURE);
	}
    
    char outBuf[SHM_SIZE];
    uint32_t outLen;

    // ОСНОВНАЯ ЧАСТЬ

    while (true) {
        sem_wait(semFull);
        
        uint32_t len = shmBuf->length;
        
        if (len == 0) {
            sem_post(semEmpty);
            break;
        }

        work(shmBuf->data, len, outBuf, &outLen);

        if (outLen > 0) {
            write(file, outBuf, outLen);
            write(file, "\n", 1);
        }
        
        sem_post(semEmpty);
    }

    sem_close(semFull);
    sem_close(semEmpty);
    munmap(shmBuf, SHM_SIZE);
    close(file);

    return EXIT_SUCCESS;
}
