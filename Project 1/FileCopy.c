/*
 * FileCopy.c
 * 
 * This program opens a file and writes its contents to a pipe 
 */
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define BUFFER_SIZE 512

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        return 1;
    }

    char *source_file = argv[1];
    char *destination_file = argv[2];

    // Create a pipe with inheritable handles
    HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;  // Make the pipe handles inheritable
    sa.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        fprintf(stderr, "Failed to create pipe. Error code: %lu\n", GetLastError());
        return 1;
    }

    // Ensure the write handle is not inherited
    if (!SetHandleInformation(hWritePipe, HANDLE_FLAG_INHERIT, 0)) {
        fprintf(stderr, "Failed to set handle information. Error code: %lu\n", GetLastError());
        return 1;
    }

    // Set up STARTUPINFO for the child process
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    si.hStdInput = hReadPipe;  // Child's stdin is the read end of the pipe
    si.dwFlags |= STARTF_USESTDHANDLES;

    // Open the source file
    FILE *src_fp = fopen(source_file, "rb");
    if (!src_fp) {
        fprintf(stderr, "Failed to open source file. Error code: %lu\n", GetLastError());
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return 1;
    }

    // Create the child process
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "child.exe \"%s\"", destination_file);

    if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        fprintf(stderr, "Failed to create child process. Error code: %lu\n", GetLastError());
        fclose(src_fp);
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return 1;
    }

    printf("Parent process created pipe and started writing.\n");

    // Close the read end of the pipe in the parent process
    CloseHandle(hReadPipe);

    // Read from the source file and write to the pipe
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    DWORD bytes_written;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, src_fp)) > 0) {
        if (!WriteFile(hWritePipe, buffer, bytes_read, &bytes_written, NULL)) {
            fprintf(stderr, "Error writing to pipe. Error code: %lu\n", GetLastError());
            break;
        }

        if (bytes_written != bytes_read) {
            fprintf(stderr, "Partial write to pipe: written %lu, expected %lu\n", bytes_written, bytes_read);
            break;
        }
    }

    fclose(src_fp);
    CloseHandle(hWritePipe);  // Close the write end of the pipe after writing

    // Wait for the child process to finish
    WaitForSingleObject(pi.hProcess, INFINITE);

    printf("Parent process finished writing and waited for child.\n");

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
