#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

// 1 <a> <b> <step> = function 1
// 2 <x> = function 2
// q = end
// 0 = switch

typedef float (*sin_t)(float, float, float);
typedef float (*e_t)(int);

static sin_t curr_sin_func = NULL;
static e_t curr_e_func = NULL;
static int curr_version = 2;

static void *loaded_sin = NULL;
static void *loaded_e = NULL;

const char *SIN_FUNC_1_NAME = "./libfunc_sin_1.so";
const char *SIN_FUNC_2_NAME = "./libfunc_sin_2.so";

const char *E_FUNC_1_NAME = "./libfunc_e_1.so";
const char *E_FUNC_2_NAME = "./libfunc_e_2.so";

int load_func(void **library, void **function, const char *lib_name, const char *func_name)
{
    *library = dlopen(lib_name, RTLD_LAZY);
    if (*library == NULL) {
        const char msg[] = "Ошибка: dlopen. \n";
		write(STDERR_FILENO, msg, sizeof(msg));
        return 0;
    }

    *function = dlsym(*library, func_name);
    if (*function == NULL) {
        const char msg[] = "Ошибка: dlsym. \n";
		write(STDERR_FILENO, msg, sizeof(msg));
        return 0;
    }

    return 1;
}

int switch_realization()
{
    if (loaded_sin) {
        dlclose(loaded_sin);
    }
    if (loaded_e) {
        dlclose(loaded_e);
    }

    curr_version = (curr_version == 1) ? 2 : 1;
    const char *sin_lib_name = (curr_version == 1) ? SIN_FUNC_1_NAME : SIN_FUNC_2_NAME;
    const char *e_lib_name = (curr_version == 1) ? E_FUNC_1_NAME : E_FUNC_2_NAME;
    

    int success_sin = load_func(&loaded_sin, (void **)&curr_sin_func, sin_lib_name, "sin_integral");
    int success_e = load_func(&loaded_e, (void **)&curr_e_func, e_lib_name, "e");

    if (success_e && success_sin) {
        char output[256];
        int bytes = snprintf(output, sizeof(output) - 1, "Переключено на реализацию №%d. \n", curr_version);
        write(STDOUT_FILENO, output, bytes);
        return 1;

    } else {
        const char msg[] = "Ошибка: не удалось поменять реализацию контрактов. \n";
		write(STDERR_FILENO, msg, sizeof(msg));
        curr_e_func = NULL;
        curr_sin_func = NULL;
        return 0;
    }
}

int main() {
    char buff[1024];
    int bytes;

    switch_realization();
    
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

        if (strcmp(buff, "0") == 0) {
            switch_realization();
            continue;
        }

        readAmount = sscanf(buff, "%d %f %f %f", &command, &a, &b, &step);

        if (command == 1 && readAmount == 4) {
            if (!curr_sin_func) {
                if (loaded_sin) {
                    dlclose(loaded_sin);
                }
                if (loaded_e) {
                    dlclose(loaded_e);
                }
                const char msg[] = "Ошибка: не удалось загрузить функцию. \n";
                write(STDERR_FILENO, msg, sizeof(msg));
                curr_e_func = NULL;
                return 0;
            }
            float res = curr_sin_func(a, b, step);
            char output[256];
            int len = snprintf(output, sizeof(output) - 1, "Результат (sin): %.10f \n", res);
            write(STDOUT_FILENO, output, len);
        }

        readAmount = sscanf(buff, "%d %d", &command, &x);

        if (command == 2 && readAmount == 2) {
            if (!curr_e_func) {
                if (loaded_sin) {
                    dlclose(loaded_sin);
                }
                if (loaded_e) {
                    dlclose(loaded_e);
                }
                const char msg[] = "Ошибка: не удалось загрузить функцию. \n";
                write(STDERR_FILENO, msg, sizeof(msg));
                curr_sin_func = NULL;
                return 0;
            }
            float res = curr_e_func(x);
            char output[256];
            int len = snprintf(output, sizeof(output) - 1, "Результат (e): %.10f \n", res);
            write(STDOUT_FILENO, output, len);
        }
    }

    if (loaded_sin) {
        dlclose(loaded_sin);
    }
    if (loaded_e) {
        dlclose(loaded_e);
    }

    return 0;
}