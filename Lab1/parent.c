#include <stdint.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

static char CHILD1_PROGRAM_NAME[] = "child1";
static char CHILD2_PROGRAM_NAME[] = "child2";

int main(int argc, char* argv[]) {
    // Получение директории файла

	char progpath[1024];
	{
		ssize_t len = readlink("/proc/self/exe", progpath,
		                       sizeof(progpath) - 1);
		if (len == -1) {
			const char msg[] = "error: failed to read full program path\n";
			write(STDERR_FILENO, msg, sizeof(msg));
			exit(EXIT_FAILURE);
		}

		while (progpath[len] != '/')
			--len;

		progpath[len] = '\0';
	}

    // Считывание имён для файлов

    char filePath1[512];
    char filePath2[512];

    ssize_t bytes1 = read(STDIN_FILENO, filePath1, sizeof(filePath1) - 1);
    if (bytes1 > 0) {
        filePath1[bytes1] = '\0';
        if (filePath1[bytes1 - 1] == '\n') {
            filePath1[bytes1 - 1] = '\0';
        }
    }

    ssize_t bytes2 = read(STDIN_FILENO, filePath2, sizeof(filePath2) - 1);
    if (bytes2 > 0) {
        filePath2[bytes2] = '\0';
        if (filePath2[bytes2 - 1] == '\n') {
            filePath2[bytes2 - 1] = '\0';
        }
    }

    // Создание pipe'ов

    int pipe1[2]; // К child1
    if (pipe(pipe1) == -1) {
        const char msg[] = "error: failed to create pipe\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    }

    int pipe2[2]; // К child2
    if (pipe(pipe2) == -1) {
        const char msg[] = "error: failed to create pipe\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    }
    
    // Процесс child1

    const pid_t child1 = fork();

    switch (child1) {
    case -1: {
        const char msg[] = "error: failed to spawn new process\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    } break;

    case 0: {
        {
            close(pipe1[1]);
            dup2(pipe1[0], STDIN_FILENO);
            close(pipe1[0]);
            
            close(pipe2[0]);
            close(pipe2[1]);

            char path[1024];
            snprintf(path, sizeof(path) - 1, "%s/%s", progpath, CHILD1_PROGRAM_NAME);
            
            char *const args[] = {CHILD1_PROGRAM_NAME, filePath1, NULL};

            int32_t status = execv(path, args);

			if (status == -1) {
				const char msg[] = "error: failed to exec into new exectuable image\n";
				write(STDERR_FILENO, msg, sizeof(msg));
				exit(EXIT_FAILURE);
			}

        } 

    } break;

    }

    // Процесс child2

    const pid_t child2 = fork();

    switch (child2) {
    case -1: {
        const char msg[] = "error: failed to spawn new process\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    } break;

    case 0: {
        {
            close(pipe2[1]);
            dup2(pipe2[0], STDIN_FILENO);
            close(pipe2[0]);

            close(pipe1[0]);
            close(pipe1[1]);

            char path[1024];
            snprintf(path, sizeof(path) - 1, "%s/%s", progpath, CHILD2_PROGRAM_NAME);
            
            char *const args[] = {CHILD2_PROGRAM_NAME, filePath2, NULL};

            int32_t status = execv(path, args);

			if (status == -1) {
				const char msg[] = "error: failed to exec into new exectuable image\n";
				write(STDERR_FILENO, msg, sizeof(msg));
				exit(EXIT_FAILURE);
			}

        } 

    } break;
    }

    close(pipe1[0]);
    close(pipe2[0]);

    // Обработка ввода пользователя

    char buf[4096];
    int counter = 0;
	ssize_t bytes;
    while (bytes = read(STDIN_FILENO, buf, sizeof(buf))) {
        if (bytes < 0) {
            const char msg[] = "error: failed to read from stdin\n";
            write(STDERR_FILENO, msg, sizeof(msg));
            exit(EXIT_FAILURE);
        } else if (buf[0] == '\n') {
            break;
        }
        
        counter++;

        if (counter % 2 == 1) {
            write(pipe1[1], buf, bytes);
        } else {
            write(pipe2[1], buf, bytes);
        }
    }

    close(pipe1[1]);
    close(pipe2[1]);

    // Ожидание дочерних процессов

    wait(NULL);
}