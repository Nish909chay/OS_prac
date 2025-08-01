#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

void print(const char *s) {
    write(1, s, strlen(s));
}

void show_error(const char *desc) {
    print(desc);
    print(": ");
    print(strerror(errno));
    print("\n");
}

int main() {
    int fd;

    // 1. File doesn't exist
    fd = open("nonexistent.txt", O_RDONLY);
    if (fd == -1) show_error("Error opening nonexistent.txt");

    // 2. Permission denied
    fd = open("/etc/shadow", O_WRONLY); // Usually restricted
    if (fd == -1) show_error("Error opening /etc/shadow");

    // 3. Trying to write to a directory
    fd = open("/", O_WRONLY);
    if (fd == -1) show_error("Error opening directory '/' with O_WRONLY");

    return 0;
}