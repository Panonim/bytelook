#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>
#include <string.h>
#include <unistd.h>

#define CYAN "\033[1;36m"
#define RESET "\033[0m"

void print_size(unsigned long size_in_bytes) {
    double size_in_mb = size_in_bytes / (1024.0 * 1024.0);
    char buffer[50];
    if (size_in_mb > 1024) {
        snprintf(buffer, sizeof(buffer), "%.2f GB", size_in_mb / 1024.0);
    } else {
        snprintf(buffer, sizeof(buffer), "%.2f MB", size_in_mb);
    }
    printf("%s\n", buffer);
}

void print_disk_usage(const char **paths, int num_paths) {
    printf("%s┌───────────────────────────────────┐%s\n", CYAN, RESET);

    for (int i = 0; i < num_paths; ++i) {
        struct statvfs stat;

        if (statvfs(paths[i], &stat) != 0) {
            perror("statvfs");
            continue;
        }

        printf("%s│ Path: %-29s │%s\n", CYAN, paths[i], RESET);
        printf("%s│ Total: ", CYAN);
        print_size(stat.f_blocks * stat.f_frsize);
        printf("%s│ Used:  ", CYAN);
        print_size((stat.f_blocks - stat.f_bfree) * stat.f_frsize);
        printf("%s│ Available: ", CYAN);
        print_size(stat.f_bavail * stat.f_frsize);

        if (i < num_paths - 1) {
            printf("%s│                                   %s\n", CYAN, RESET);
        }
    }

    printf("%s└───────────────────────────────────┘%s\n\n", CYAN, RESET);
}

int main() {
    const char *home = getenv("HOME");
    const char *paths[] = {"/home", "/media", home};
    int num_paths = sizeof(paths) / sizeof(paths[0]);

    // Print top square border
    printf("%s┌───────────────────────────────────┐\n", CYAN);
    printf("%s│                                   │\n", CYAN);
    printf("%s│             ByteLook              │\n", CYAN);
    printf("%s│                                   │\n", CYAN);
    printf("%s└───────────────────────────────────┘%s\n", CYAN, RESET);

    // Print disk usage information
    print_disk_usage(paths, num_paths);

    return 0;
}
