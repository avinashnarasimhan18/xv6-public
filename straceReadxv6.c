#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main() {
    int fd;

    // Attempt to open a non-existent file for reading
    fd = open("nonexistent.txt", O_RDONLY);
    if (fd < 0) {
        printf(1, "Error: Could not open nonexistent.txt for reading\n");
    }

    exit();
}
