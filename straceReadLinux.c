#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd;

    // Attempt to open a non-existent file for reading
    fd = open("nonexistent.txt", O_RDONLY);
    if (fd < 0) {
        perror("Error opening nonexistent.txt");
    }

    return 0;
}
