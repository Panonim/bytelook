#!/bin/bash

# Function to check if a command is available
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check if GCC is installed
if ! command_exists gcc; then
    echo "GCC is not installed. Please install GCC to proceed."
    exit 1
fi

# Check if curl is installed
if ! command_exists curl; then
    echo "curl is not installed. Please install curl to proceed."
    exit 1
fi

# Download the C source code file
curl -o bytelook.c https://raw.githubusercontent.com/Panonim/bytelook/main/bytelook.c

# Check if the download was successful
if [ $? -ne 0 ]; then
    echo "Failed to download the source code file. Please check your internet connection or try again later."
    exit 1
fi

# Compile the C program
gcc bytelook.c -o bytelook

# Check if compilation was successful
if [ $? -ne 0 ]; then
    echo "Failed to compile the source code file. Please check for any compilation errors and try again."
    exit 1
fi

# Move the compiled binary to /usr/local/bin
sudo mv bytelook /usr/local/bin/ || { echo "Failed to move the binary to /usr/local/bin. Please make sure you have the necessary permissions."; exit 1; }

echo "ByteLook has been installed successfully! You can now use the 'bytelook' command."

# Remove the source code file
rm bytelook.c
