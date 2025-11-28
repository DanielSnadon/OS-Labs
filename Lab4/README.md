# ЛР №3 / Вариант 18
### Федосов Д.А. М8О-212БВ-24

#### Инструкции запуска:

##### Сборка:

```bash
$ gcc -o libfunc_sin_1.so -shared -fPIC func_sin_1.c -lm |
gcc -o libfunc_sin_2.so -shared -fPIC func_sin_2.c -lm |
gcc -o libfunc_e_1.so -shared -fPIC func_e_1.c -lm |
gcc -o libfunc_e_2.so -shared -fPIC func_e_2.c -lm
$ gcc -o static stat.c -L. -lfunc_e_1 -lfunc_sin_1 -lm -Wl,-rpath=.
$ gcc -o dynamic dynam.c -ldl
```

##### Запуск:

```bash
# Для статических библиотек:
./static
# Для динамических библиотек:
./dynamic
```

###### (Отчёт находится в папке docs)