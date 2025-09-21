#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

void print_fd(int fd) {
    char buf[20];
    int i = 0;
    if (fd == 0) buf[i++] = '0';
    else {
        int temp = fd, j;
        while (temp > 0) {
            buf[i++] = '0' + (temp % 10);
            temp /= 10;
        }
        for (j = 0; j < i/2; j++) {
            char t = buf[j];
            buf[j] = buf[i - 1 - j];
            buf[i - 1 - j] = t;
        }
    }
    buf[i++] = '\n';
    write(1, buf, i);
}

int main(int argc, char *argv[]) {
    if (argc < 4) return 1;

    int fd1 = open(argv[1], O_RDONLY);
    int fd2 = open(argv[2], O_RDONLY);
    int fd3 = open(argv[3], O_RDONLY);

    print_fd(fd1);
    print_fd(fd2);
    print_fd(fd3);

    close(fd1);
    close(fd2);
    close(fd3);

    return 0;
}
