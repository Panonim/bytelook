#!/bin/bash

# Function to check if a command is available
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check if curl is installed
if ! command_exists curl; then
    echo "curl is not installed. Please install curl to proceed with the installation."
    exit 1
fi

# Function to check if a directory is in the PATH
path_contains() {
    local target="$1"
    local path
    IFS=':' read -ra path <<< "$PATH"
    for dir in "${path[@]}"; do
        [[ "$dir" == "$target" ]] && return 0
    done
    return 1
}

# Check if the user has permission to copy to /usr/local/bin
if [[ ! -w "/usr/local/bin" ]]; then
    echo "You don't have permission to copy files to /usr/local/bin. Please run this script as root or with sudo."
    exit 1
fi

# Check if the directory is in the PATH
if ! path_contains "/usr/local/bin"; then
    echo "The directory /usr/local/bin is not in your PATH. Please add it to your PATH before proceeding."
    exit 1
fi

# Download the compiled binary file
curl -o compiled https://raw.githubusercontent.com/Panonim/bytelook/main/bytelook.c

# Make the binary executable
chmod +x compiled

# Move the binary to /usr/local/bin
sudo mv compiled /usr/local/bin

echo "Installation complete! You can now use the 'compiled' command."
