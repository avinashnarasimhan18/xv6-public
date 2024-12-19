#include "types.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf(2, "Usage: strace on|off|run|dump|-o <filename>\n");
        exit();
    }

    if (strcmp(argv[1], "on") == 0) {
        strace(1);
    } else if (strcmp(argv[1], "off") == 0) {
        strace(0);
    } else if (strcmp(argv[1], "run") == 0) {
        if (argc < 3) {
            printf(2, "Usage: strace run <command>\n");
            exit();
        }
        int pid = fork();
        if (pid == 0) {
            strace(1);
            exec(argv[2], &argv[2]);
            printf(2, "exec failed\n");
            exit();
        } else {
            wait();
        }
    } else if (strcmp(argv[1], "dump") == 0) {
        strace_dump();  // Call the existing system call to dump the buffer
    } else if (strcmp(argv[1], "-o") == 0) {
        if (argc < 3) {
            printf(2, "Usage: strace -o <filename>\n");
            exit();
        }

        // Open the specified file for appending
        int fd = open(argv[2], O_WRONLY | O_CREATE);
        if (fd < 0) {
            printf(2, "Error: Cannot open file %s\n", argv[2]);
            exit();
        }

        // Redirect stdout to the file
        close(1);  // Close the current stdout
        dup(fd);   // Duplicate fd so it becomes the new stdout
        close(fd); // Close the original fd

        // Dump trace logs to the file
        strace_dump();

        printf(2, "Trace logs appended to %s\n", argv[2]);
        exit();
    } else {
        printf(2, "Usage: strace on|off|run|dump|-o <filename>\n");
    }

    exit();
}
