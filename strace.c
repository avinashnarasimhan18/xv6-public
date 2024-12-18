#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf(2, "Usage: strace on|off|run|dump <command>\n");
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
        strace_dump();  // Call the new system call to dump the buffer
    } else {
        printf(2, "Usage: strace on|off|run|dump <command>\n");
    }

    exit();
}
