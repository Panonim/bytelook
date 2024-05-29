#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>
#include <string.h>
#include <unistd.h>

#define CYAN "\033[1;36m"
#define RESET "\033[0m"

// Function to print size in a human-readable format
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

// Function to print disk usage information for given paths
void print_disk_usage(const char **paths, int num_paths, int no_home) {
    printf("%s┌───────────────────────────────────┐%s\n", CYAN, RESET);

    for (int i = 0; i < num_paths; ++i) {
        struct statvfs stat;

        // Check if statvfs succeeds
        if (statvfs(paths[i], &stat) != 0) {
            // Print error message if statvfs fails
            printf("%s│ Path: %-29s │%s\n", CYAN, paths[i], RESET);
            printf("%s│ Error: Unable to access path      │%s\n", CYAN, RESET);
            if (i < num_paths - 1) {
                printf("%s│                                   %s\n", CYAN, RESET);
            }
            continue;
        }

        // Print disk usage details
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

    // Print message if the user does not have a home directory
    if (no_home) {
        printf("%s│                                   │%s\n", CYAN, RESET);
        printf("%s│ User doesn't have a home directory │%s\n", CYAN, RESET);
    }

    printf("%s└───────────────────────────────────┘%s\n\n", CYAN, RESET);
}

int main() {
    const char *paths[4] = {"/media", "/home", "/"};
    int num_paths = 3;

    // Check if the home directory exists
    const char *home_dir = getenv("HOME");
    int no_home = 0;

    // Add home directory to paths if it exists
    if (home_dir) {
        paths[num_paths++] = home_dir;
    } else {
        no_home = 1;
    }
    
    printf("%s┌───────────────────────────────────┐\n", CYAN);
    printf("%s│                                   │\n", CYAN);
    printf("%s│             ByteLook              │\n", CYAN);
    printf("%s│                                   │\n", CYAN);
    printf("%s└───────────────────────────────────┘%s\n", CYAN, RESET);

    // Print disk usage information
    print_disk_usage(paths, num_paths, no_home);

    return 0;
}
