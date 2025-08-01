#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void print(const char *s) {
    write(1, s, strlen(s));
}

int get_word(char *word, int max_len) {
    char c;
    int i = 0;
    while (i < max_len - 1 && read(0, &c, 1) > 0) {
        if (c == '\n') break;
        word[i++] = c;
    }
    word[i] = '\0';
    return i;
}

int contains(const char *line, const char *word) {
    int i, j;
    int n = strlen(line), m = strlen(word);
    for (i = 0; i <= n - m; i++) {
        for (j = 0; j < m; j++) {
            if (line[i + j] != word[j])
                break;
        }
        if (j == m) return 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        const char *msg = "Usage: ./simplegrep <filename>\n";
        write(2, msg, strlen(msg));
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        write(2, "Error opening file\n", 19);
        return 1;
    }

    char word[100];
    print("Enter word to search: ");
    get_word(word, sizeof(word));

    char buf[1];
    char line[1024];
    int li = 0;

    while (read(fd, buf, 1) > 0) {
        if (buf[0] == '\n' || li >= 1023) {
            line[li] = '\0';
            if (contains(line, word)) {
                write(1, line, strlen(line));
                write(1, "\n", 1);
            }
            li = 0;
        } else {
            line[li++] = buf[0];
        }
    }

    if (li > 0) {
        line[li] = '\0';
        if (contains(line, word)) {
            write(1, line, strlen(line));
            write(1, "\n", 1);
        }
    }

    close(fd);
    return 0;
}
