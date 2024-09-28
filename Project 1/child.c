/*
 * child.c
 *
 * This program reads the contents of the pipe and writes it to a file.
 */
  
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <destination_file>\n", argv[0]);
        return 1;
    }

    char *destination_file = argv[1];

    // Open the destination file with write permissions
    FILE *dest_fp = fopen(destination_file, "wb");
    if (!dest_fp) {
        fprintf(stderr, "Failed to open destination file. Error code: %lu\n", GetLastError());
        return 1;
    }

    printf("Child process started. Ready to read from pipe.\n");

    // Read from the pipe (standard input) and write to the destination file
    char buffer[BUFFER_SIZE];
    DWORD bytes_read;
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);

    if (hInput == INVALID_HANDLE_VALUE || hInput == NULL) {
        fprintf(stderr, "Invalid input handle in child process. Error code: %lu\n", GetLastError());
        fclose(dest_fp);
        return 1;
    }

    while (ReadFile(hInput, buffer, BUFFER_SIZE, &bytes_read, NULL) && bytes_read > 0) {
        fwrite(buffer, 1, bytes_read, dest_fp);
    }

    if (bytes_read == 0) {
        printf("Child process finished reading from pipe.\n");
    } else {
        fprintf(stderr, "Error reading from pipe in child process. Error code: %lu\n", GetLastError());
    }

    fclose(dest_fp);
    printf("Child process finished writing to %s.\n", destination_file);

    return 0;
}
