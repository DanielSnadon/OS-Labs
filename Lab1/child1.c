#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {

    // Получение директории файла

    char filePath[512];
    strcpy(filePath, argv[1]);

    // Открытие файла для записи

    int32_t file = open(filePath, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0600);
    if (file == -1) {
		const char msg[] = "error: failed to open requested file\n";
		write(STDERR_FILENO, msg, sizeof(msg));
		exit(EXIT_FAILURE);
	}

    // Обработка строк

    char buf[4096];
    char outBuf[4096];
    ssize_t bytes;
    const char banwords[] = {'a', 'e', 'i', 'o', 'u', 'y', 'A', 'E', 'I', 'O', 'U', 'Y'};
    
    while (bytes = read(STDIN_FILENO, buf, sizeof(buf))) {
        if (bytes < 0) {
			const char msg[] = "error: failed to read from stdin\n";
			write(STDERR_FILENO, msg, sizeof(msg));
			exit(EXIT_FAILURE);
		}

        int anotherCounter = 0;
        for (uint32_t i = 0; i < bytes; ++i) {
            if (memchr(banwords, buf[i], 12) == NULL) {
                outBuf[anotherCounter++] = buf[i];
            }
        }

        if (anotherCounter > 0) {
            write(file, outBuf, anotherCounter);
        }
    }
    
    close(file);
}