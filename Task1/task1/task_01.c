#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

void execute_command_with_delay(const char *line) {
    int delay;
    char command[MAX_LINE_LENGTH];

    // Читаем задержку и команду из строки
    sscanf(line, "%d %[^\n]", &delay, command);

    // Задержка перед выполнением команды
    sleep(delay);

    // Выполнение команды
    system(command);
}

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
    while (fgets(line, sizeof(line), file)) {
        execute_command_with_delay(line);
    }

    fclose(file);
    return 0;
}