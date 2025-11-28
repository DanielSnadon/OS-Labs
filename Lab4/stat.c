#include <unistd.h>
#include <string.h>
#include "functions.h"
#include <stdio.h>

// 1 <a> <b> <step> = function 1
// 2 <x> = function 2
// q = end

int main() {
    char buff[1024];
    int bytes;

    while ((bytes = read(STDIN_FILENO, buff, sizeof(buff) - 1)) > 0) {

        buff[bytes] = '\0';

        if (buff[bytes - 1] == '\n') {
            buff[bytes - 1] = '\0';
        }

        int command;
        float a, b, step;
        int x;
        int readAmount;
        
        if (strcmp(buff, "q") == 0) {
            break;
        }

        readAmount = sscanf(buff, "%d %f %f %f", &command, &a, &b, &step);

        if (command == 1 && readAmount == 4) {
            float res = sin_integral(a, b, step);
            char output[256];
            int len = snprintf(output, sizeof(output) - 1, "Результат (sin): %.10f \n", res);
            write(STDOUT_FILENO, output, len);
        }

        readAmount = sscanf(buff, "%d %d", &command, &x);

        if (command == 2 && readAmount == 2) {
            float res = e(x);
            char output[256];
            int len = snprintf(output, sizeof(output) - 1, "Результат (e): %.10f \n", res);
            write(STDOUT_FILENO, output, len);
        }
    }

    return 0;
}