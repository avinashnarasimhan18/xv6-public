#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]) {
    int i;
    int num_children = 3;

    if (argc > 1) {
        num_children = atoi(argv[1]);  // Accept the number of child processes
    }

    printf(1, "Parent pid: %d\n", getpid());

    for (i = 0; i < num_children; i++) {
        int pid = fork();
        if (pid == 0) {
            // Child process
            printf(1, "Child pid: %d\n", getpid());
            printf(1, "Child %d: Executing 'echo hello'\n", getpid());
            exec("echo", (char *[]){"echo", "hello", 0});
            printf(1, "Child %d: exec failed\n", getpid());
            exit();
        } else if (pid > 0) {
            // Parent process
            wait();
        } else {
            printf(2, "fork failed\n");
            exit();
        }
    }

    printf(1, "All children finished. Exiting parent process.\n");
    exit();
}
