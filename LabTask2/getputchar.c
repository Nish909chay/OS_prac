
#include <unistd.h>

char mygetchar() {
    char c;
    int r = read(0, &c, 1);
    if (r <= 0) return -1; // EOF or error
    return c;
}

void myputchar(char c) {
    write(1, &c, 1);
}

int main() {
    char c;
    while ((c = mygetchar()) != -1) {
        myputchar(c);
    }
    return 0;
}