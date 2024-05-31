#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>
#include <string.h>
#include <unistd.h>

#define CYAN "\033[1;36m"
#define RESET "\033[0m"

// Function to print size in a human format
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

// Function to print disk usage information
void print_disk_usage(const char **paths, int num_paths, int no_home) {
    printf("%s┌───────────────────────────────────┐%s\n", CYAN, RESET);

    for (int i = 0; i < num_paths; ++i) {
        struct statvfs stat;

        // Check if statvfs succeeds
        if (statvfs(paths[i], &stat) != 0) {
            // Print error message if statvfs fails
            printf("%s│ Path: %-29s %s\n", CYAN, paths[i], RESET);
            printf("%s│ Error: Unable to access path      %s\n", CYAN, RESET);
            if (i < num_paths - 1) {
                printf("%s│                                   %s\n", CYAN, RESET);
            }
            continue;
        }
        // Print disk usage display
        printf("%s│ Path: %-29s %s\n", CYAN, paths[i], RESET);
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
        printf("%s│                                   %s\n", CYAN, RESET);
        printf("%s│ User doesn't have a home directory %s\n", CYAN, RESET);
    }

    printf("%s└───────────────────────────────────┘%s\n\n", CYAN, RESET);
}

// Function to update bytelook
void update_bytelook() {
    printf("Performing update...\n");
    system("curl -s -o temp_update_file.c https://raw.githubusercontent.com/Panonim/bytelook/main/bytelook.c");
    system("gcc -o temp_executable temp_update_file.c");
    system("mv temp_executable bytelook");
    system("rm temp_update_file.c");
    printf("Update complete. Please restart ByteLook.\n");
}
// Function to add a cron job
void add_cron_job() {
    FILE *cron = popen("crontab -l", "r");
    if (!cron) {
        perror("Failed to read crontab");
        return;
    }

    char cron_jobs[1024] = {0};
    fread(cron_jobs, 1, sizeof(cron_jobs), cron);
    pclose(cron);

    if (!strstr(cron_jobs, CRON_JOB)) {
        FILE *new_cron = popen("crontab -", "w");
        if (!new_cron) {
            perror("Failed to write to crontab");
            return;
        }
        fprintf(new_cron, "%s%s", cron_jobs, CRON_JOB);
        pclose(new_cron);
        printf("Auto-update cron job added.\n");
    } else {
        printf("Auto-update cron job already exists.\n");
    }
}

// Function to remove a cron job
void remove_cron_job() {
    FILE *cron = popen("crontab -l", "r");
    if (!cron) {
        perror("Failed to read crontab");
        return;
    }

    char cron_jobs[1024] = {0};
    fread(cron_jobs, 1, sizeof(cron_jobs), cron);
    pclose(cron);

    if (strstr(cron_jobs, CRON_JOB)) {
        char *position = strstr(cron_jobs, CRON_JOB);
        memmove(position, position + strlen(CRON_JOB), strlen(position + strlen(CRON_JOB)) + 1);
        FILE *new_cron = popen("crontab -", "w");
        if (!new_cron) {
            perror("Failed to write to crontab");
            return;
        }
        fprintf(new_cron, "%s", cron_jobs);
        pclose(new_cron);
        printf("Auto-update cron job removed.\n");
    } else {
        printf("Auto-update cron job does not exist.\n");
    }
}

int main(int argc, char *argv[]) {
    int auto_updates = 0;
    
    // Print help function and set the vars
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [OPTIONS]\n", argv[0]);
            printf("Options:\n");
            printf("  --help          Display this help message\n");
            printf("  -v              Display version information\n");
            printf("  update          Updates the program to the newest version\n");
            printf("  --auto-updates  Enable auto-updates (default is off)\n");
            return 0;
        } else if (strcmp(argv[i], "-v") == 0) {
            printf("ByteLook version 1.2.0\n");
            return 0;
        } else if (strcmp(argv[i], "update") == 0) {
            update_bytelook();
            return 0;
        } else if (strcmp(argv[i], "--auto-updates") == 0) {
            auto_updates = 1;
        }
    }
    
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

    // Check if auto-updates are enabled
    if (auto_updates) {
        update_bytelook();
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
