/*
    Goal
    - Runs commands without needing full paths (manually searches PATH)
    - Can change directories with "cd"
    - Shows a dynamic prompt (default = current directory)
    - Lets you change prompt with PS1=...
    - Lets you set your own PATH
    - Supports input/output redirection (<, >, >>)
    - Exit with "exit" or Ctrl+D
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_INPUT 1024
#define MAX_ARGS 128
#define MAX_PATHS 64
#define MAX_PROMPT 256

char *shell_paths[MAX_PATHS];
int path_count = 0;
char prompt[MAX_PROMPT];

void free_paths();
void update_prompt_cwd();
char *find_executable(const char *cmd);
void init_default_path();
void set_path(const char *path_str);
void run_command(char *line);

int main() {
    init_default_path();
    update_prompt_cwd();    // nishchay@ubuntu:~/projects$

    char input_line[MAX_INPUT];

    while (1) {
        printf("%s", prompt);
        fflush(stdout);     // forces data out of buffer as stdout is 
        // line buffered so unless buffer is full or it finds \n the output is not written 

        if (fgets(input_line, sizeof(input_line), stdin) == NULL) {
            printf("\nBye!\n");
            break;
        }

        input_line[strcspn(input_line, "\n")] = 0;

        if (strlen(input_line) == 0) continue;

        run_command(input_line);
    }

    free_paths();
    return 0;
}

void run_command(char *line) {
    char *args[MAX_ARGS];
    char line_copy[MAX_INPUT];

    strncpy(line_copy, line, MAX_INPUT);
    line_copy[MAX_INPUT - 1] = '\0';

    int argc = 0;
    char *token = strtok(line_copy, " \t");
    while (token && argc < MAX_ARGS - 1) {
        args[argc++] = token;
        token = strtok(NULL, " \t");
    }
    args[argc] = NULL;
    if (argc == 0) return;

    if (strcmp(args[0], "exit") == 0) {
        printf("Bye!\n");
        exit(0);
    }

    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "cd: where do you want to go?\n");
        } else if (chdir(args[1]) != 0) {
            perror("cd");
        }
        return;
    }

    if (strncmp(line, "PS1=", 4) == 0) {
        const char *val = line + 4;
        if (strcmp(val, "\\w$") == 0) {
            update_prompt_cwd();
        } else {
            strncpy(prompt, val, sizeof(prompt));
            prompt[sizeof(prompt) - 1] = '\0';
        }
        return;
    }

    if (strncmp(line, "PATH=", 5) == 0) {
        set_path(line);
        return;
    }

    int in_fd = -1, out_fd = -1, append_flag = 0;
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0) {
            if (args[i+1] == NULL) {
                fprintf(stderr, "No file after '<'\n");
                return;
            }
            in_fd = open(args[i+1], O_RDONLY);
            if (in_fd < 0) { perror("open"); return; }
            args[i] = NULL;
        } else if (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0) {
            if (args[i+1] == NULL) {
                fprintf(stderr, "No file after '>'\n");
                return;
            }
            append_flag = (strcmp(args[i], ">>") == 0);
            int flags = O_WRONLY | O_CREAT | (append_flag ? O_APPEND : O_TRUNC);
            out_fd = open(args[i+1], flags, 0644);
            if (out_fd < 0) { perror("open"); return; }
            args[i] = NULL;
        }
    }

    char *cmd_path = find_executable(args[0]);
    if (!cmd_path) {
        fprintf(stderr, "%s: command not found\n", args[0]);
        return;
    }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); free(cmd_path); return; }

    if (pid == 0) {
        if (in_fd != -1) { dup2(in_fd, STDIN_FILENO); close(in_fd); }
        if (out_fd != -1) { dup2(out_fd, STDOUT_FILENO); close(out_fd); }

        execv(cmd_path, args);
        perror("exec failed");
        exit(1);
    } else {
        if (in_fd != -1) close(in_fd);
        if (out_fd != -1) close(out_fd);
        int status;
        waitpid(pid, &status, 0);
    }

    free(cmd_path);
}

void update_prompt_cwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        snprintf(prompt, sizeof(prompt), "%s$ ", cwd);
    } else {
        strcpy(prompt, "$ ");
    }
}

void set_path(const char *path_str) {
    free_paths();
    const char *val = strchr(path_str, '=');
    if (!val) { fprintf(stderr, "Bad PATH format.\n"); return; }
    val++;

    char *paths_copy = strdup(val);
    char *token = strtok(paths_copy, ":");
    path_count = 0;
    while (token && path_count < MAX_PATHS) {
        shell_paths[path_count++] = strdup(token);
        token = strtok(NULL, ":");
    }
    free(paths_copy);
}

void init_default_path() {
    const char *default_path = "/bin:/usr/bin";
    char *paths_copy = strdup(default_path);
    char *token = strtok(paths_copy, ":");  // split the token based on delimiters ":"
    path_count = 0;
    while (token && path_count < MAX_PATHS) {
        shell_paths[path_count++] = strdup(token);
        token = strtok(NULL, ":");
    }   // parses "/bin:/usr/bin" into 2 usable paths - /bin and /use/bin
    free(paths_copy);
}

char *find_executable(const char *cmd) {
    if (strchr(cmd, '/')) {
        if (access(cmd, X_OK) == 0) return strdup(cmd);
        return NULL;
    }

    for (int i = 0; i < path_count; i++) {
        int len = strlen(shell_paths[i]) + strlen(cmd) + 2;
        char *full_path = malloc(len);
        snprintf(full_path, len, "%s/%s", shell_paths[i], cmd);
        if (access(full_path, X_OK) == 0) return full_path;
        free(full_path);
    }
    return NULL;
}

void free_paths() {
    for (int i = 0; i < path_count; i++) {
        free(shell_paths[i]);
    }
    path_count = 0;
}
