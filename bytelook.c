#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

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
            printf("%s│                                   │%s\n", CYAN, RESET);
        }
    }

    printf("%s└───────────────────────────────────┘%s\n\n", CYAN, RESET);
}

int main() {
    FILE *fp;
    char path[1035];
    const char *paths[100];
    int num_paths = 0;

    /* Open the command for reading. */
    fp = popen("lsblk -nr -o MOUNTPOINT", "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        exit(1);
    }

    /* Read the output a line at a time - store it in paths array. */
    while (fgets(path, sizeof(path) - 1, fp) != NULL) {
        // Remove newline character
        path[strcspn(path, "\n")] = 0;

        if (strlen(path) > 0) {
            paths[num_paths++] = strdup(path);
        }
    }

    /* Close */
    pclose(fp);

    // Print top square border
    printf("%s┌───────────────────────────────────┐\n", CYAN);
    printf("%s│                                   │\n", CYAN);
    printf("%s│             ByteLook              │\n", CYAN);
    printf("%s│                                   │\n", CYAN);
    printf("%s└───────────────────────────────────┘%s\n", CYAN, RESET);

    // Print disk usage information
    print_disk_usage(paths, num_paths);

    // Free allocated memory for paths
    for (int i = 0; i < num_paths; ++i) {
        free((char *)paths[i]);
    }

    return 0;
}
