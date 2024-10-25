#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE_LENGTH 256
#define MAX_COMMAND_LENGTH 128

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <файл>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Ошибка открытия файла");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    int delay;
    char command[MAX_COMMAND_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d %[^\n]", &delay, command) != 2) {
            fprintf(stderr, "Ошибка формата строки: %s\n", line);
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) {
            // Дочерний процесс
            sleep(delay);
            char *args[MAX_COMMAND_LENGTH];
            args[0] = strtok(command, " ");
            int i = 1;
            char *arg;

            // Разделяем команду на аргументы
            while ((arg = strtok(NULL, " ")) != NULL) {
                args[i++] = arg;
            }
            args[i] = NULL;

            execvp(args[0], args); 
            perror("Ошибка выполнения команды");
            exit(1);
        }
    }

    fclose(file);
    while (wait(NULL) > 0);
    return 0;
}