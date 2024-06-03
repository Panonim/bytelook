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
void update_current_file() {
    printf("Performing update...\n");
    system("curl -s -o temp_update_file.c https://raw.githubusercontent.com/Panonim/bytelook/main/bytelook.c");
    system("gcc -o temp_executable temp_update_file.c");
    system("mv temp_executable /usr/local/bin/bytelook");
    system("rm temp_update_file.c");
    printf("Update completed successfully.\n");
}

void toggle_auto_update(const char *action, const char *program_path) {
    char cron_command[256];

    if (strcmp(action, "on") == 0) {
        // Construct the cron command to add the entry
        sprintf(cron_command, "(crontab -l 2>/dev/null; echo \"0 0 */3 * * %s update\") | crontab -", program_path);
        // Execute the command
        system(cron_command);
        printf("Auto Update: on\n");
    } else if (strcmp(action, "off") == 0) {
        // Construct the cron command to remove the entry
        sprintf(cron_command, "(crontab -l | grep -v '%s update') | crontab -", program_path);
        // Execute the command
        system(cron_command);
        printf("Auto Update: off\n");
    } else {
        printf("Invalid action. Use '--auto-update on' or '--auto-update off'.\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "update") == 0) {
        update_current_file();
        return 0;
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

    // Get the path to the bytelook
    char program_path[256];
    ssize_t len = readlink("/proc/self/exe", program_path, sizeof(program_path)-1);
    if (len != -1) {
        program_path[len] = '\0';
    } else {
        printf("Error: Unable to determine the path to the executable.\n");
        return 1;
    }
  // Function to check the current auto-update status
int check_auto_update_status(const char *program_path) {
    FILE *cron_file = popen("crontab -l", "r");
    if (cron_file == NULL) {
        return 0; // Auto-update is off
    }

    char line[256];
    while (fgets(line, sizeof(line), cron_file) != NULL) {
        if (strstr(line, program_path) != NULL && strstr(line, "update") != NULL) {
            pclose(cron_file);
            return 1; // Auto-update is on
        }
    }

    pclose(cron_file);
    return 0; // Auto-update is off
}
    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
       if (strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [OPTIONS]\n", argv[0]);
            printf("Options:\n");
            printf("  --help     Display this help message\n");
            printf("  -v         Display version information\n");
            printf("  update     Updates a program to the newest version\n");
            printf("  --auto-update on/off   Current status: %s\n", check_auto_update_status(program_path) ? "on" : "off");
            return 0;
        } else if (strcmp(argv[i], "-v") == 0) {
            printf("ByteLook version 2.0.0\n");
            return 0;
        } else if (strcmp(argv[i], "--auto-update") == 0) {
            if (i + 1 < argc) {
                toggle_auto_update(argv[i + 1], program_path);
                return 0;
            } else {
                printf("Missing argument for --auto-update. Use 'on' or 'off'.\n");
                return 0;
            }
        }
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
