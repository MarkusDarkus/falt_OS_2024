#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

void compress_file(const char *filepath) {
    char command[256];
    snprintf(command, sizeof(command), "gzip '%s'", filepath);
    system(command);
}

int should_copy(const char *src_path, const char *dest_path) {
    struct stat src_stat, dest_stat;
    
    if (stat(src_path, &src_stat) != 0) {
        perror("Ошибка при получении информации о файле");
        return 0;
    }

    if (stat(dest_path, &dest_stat) != 0) {
        // Файл назначения не существует, нужно копировать
        return 1;
    }

    // Сравниваем время последней модификации
    return difftime(src_stat.st_mtime, dest_stat.st_mtime) > 0;
}

void copy_file(const char *src_path, const char *dest_path) {
    FILE *src_file = fopen(src_path, "rb");
    if (!src_file) {
        perror("Ошибка при открытии исходного файла");
        return;
    }

    FILE *dest_file = fopen(dest_path, "wb");
    if (!dest_file) {
        perror("Ошибка при создании файла назначения");
        fclose(src_file);
        return;
    }

    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        fwrite(buffer, 1, bytes, dest_file);
    }

    fclose(src_file);
    fclose(dest_file);
    compress_file(dest_path);
}

void backup_directory(const char *src_dir, const char *dest_dir) {
    DIR *dir = opendir(src_dir);
    if (!dir) {
        perror("Ошибка при открытии каталога");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char src_path[512], dest_path[512];
        snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, entry->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, entry->d_name);

        if (entry->d_type == DT_DIR) {
            mkdir(dest_path, 0755); // Создаем каталог назначения
            backup_directory(src_path, dest_path); // Рекурсивный вызов
        } else {
            if (should_copy(src_path, dest_path)) {
                copy_file(src_path, dest_path);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <исходный каталог> <каталог назначения>\n", argv[0]);
        return EXIT_FAILURE;
    }

    backup_directory(argv[1], argv[2]);
    return EXIT_SUCCESS;
}