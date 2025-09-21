/*
    Goal
    * Handle multiple pipes {ls -l | grep ".o" | echo "MnM"}
    * Handle Ctrl-C, Ctrl-z, Fg, Bg, jobs
    * Handle "history" command
    
    single-pipe 2 marks,  multiple-pipes: 3 marks, ctrl-c 0. 
    marks, ctrl-z fg bg jobs: 1 marks, history : 0.5 mark
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define MAX_INPUT 1024
#define MAX_ARGS 128
#define MAX_PATHS 64
#define MAX_PROMPT 256
#define MAX_JOBS 64
#define MAX_CMDS 32
#define MAX_HISTORY 99

char *shell_paths[MAX_PATHS];
int path_count = 0;
char prompt[MAX_PROMPT];

// for hist
char *history[MAX_HISTORY];
int history_count = 0;

void free_paths();
void update_prompt_cwd();
void init_default_path();
void set_path(const char *path_str);
void run_command(char *line);
void sigint_handler(int sig);
void sigtstp_handler(int sig);

typedef struct {
    int job_id;
    pid_t pid;
    char command[MAX_INPUT];
    int running; // 1 = running, 0 = stopped
} Job;

Job jobs[MAX_JOBS];
int job_count = 0;

pid_t fg_pid = 0; 

int main() {
    init_default_path();
    update_prompt_cwd();

    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);

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
void run_command(char *input_line) {

    if (strlen(input_line) > 0)
    add_to_history(input_line);
    
    char line_copy[MAX_INPUT];
    strncpy(line_copy, input_line, sizeof(line_copy));
    line_copy[sizeof(line_copy)-1] = '\0';

    char *argv[MAX_ARGS];
    int argc = 0;
    char *tok = strtok(line_copy, " \t");
    while (tok && argc < MAX_ARGS-1) {
        argv[argc++] = tok;
        tok = strtok(NULL, " \t");
    }
    argv[argc] = NULL;
    if (argc == 0) return;

    if (strcmp(argv[0], "history") == 0) 
    { 
        show_history(); 
        return; 
    }

    // Check for background process
    int background = 0;
    if (argc > 0 && strcmp(argv[argc-1], "&") == 0) {
        background = 1;
        argv[argc-1] = NULL; // remove '&'
        argc--;
    }

    // Built-in commands
    if (strcmp(argv[0], "exit") == 0) { printf("Bye!\n"); exit(0); }
    if (strcmp(argv[0], "cd") == 0) {
        if (!argv[1]) fprintf(stderr, "cd: no destination specified\n");
        else if (chdir(argv[1]) != 0) perror("cd");
        return;
    }
    if (strcmp(argv[0], "jobs") == 0) { list_jobs(); return; }
    if (strncmp(argv[0], "fg", 2) == 0) { bring_fg(atoi(argv[1])); return; }
    if (strncmp(argv[0], "bg", 2) == 0) { resume_bg(atoi(argv[1])); return; }

    // Pipe handling
    if (strchr(input_line, '|') != NULL) {
        execute_pipeline(input_line);
        return;
    }

    // Single command with I/O redirection
    int input_fd = -1, output_fd = -1, append_mode = 0;
    for (int i = 0; argv[i] != NULL; i++) {
        if (strcmp(argv[i], "<") == 0) {
            if (!argv[i+1]) { fprintf(stderr, "Missing file after '<'\n"); return; }
            input_fd = open(argv[i+1], O_RDONLY);
            if (input_fd < 0) { perror("open"); return; }
            argv[i] = NULL;
        } else if (strcmp(argv[i], ">") == 0 || strcmp(argv[i], ">>") == 0) {
            if (!argv[i+1]) { fprintf(stderr, "Missing file after '>'\n"); return; }
            append_mode = (strcmp(argv[i], ">>") == 0);
            int flags = O_WRONLY | O_CREAT | (append_mode ? O_APPEND : O_TRUNC);
            output_fd = open(argv[i+1], flags, 0644);
            if (output_fd < 0) { perror("open"); return; }
            argv[i] = NULL;
        }
    }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return; }

    if (pid == 0) { // Child
        if (input_fd != -1) { dup2(input_fd, STDIN_FILENO); close(input_fd); }
        if (output_fd != -1) { dup2(output_fd, STDOUT_FILENO); close(output_fd); }

        execute_command(argv, -1, -1); // your existing function
    } else { // Parent
        if (background) {
            // Add to background jobs table
            if (job_count < MAX_JOBS) {
                jobs[job_count].job_id = job_count + 1;
                jobs[job_count].pid = pid;
                strncpy(jobs[job_count].command, input_line, MAX_INPUT);
                jobs[job_count].running = 1;
                job_count++;
                printf("[%d] %d\n", job_count, pid);
            }
        } else {
            fg_pid = pid;

            int status;
            pid_t wpid = waitpid(fg_pid, &status, WUNTRACED);  // catch stopped child

            if (wpid > 0 && WIFSTOPPED(status)) {
                // Add stopped job to jobs list
                if (job_count < MAX_JOBS) {
                    jobs[job_count].job_id = job_count + 1;
                    jobs[job_count].pid = fg_pid;
                    strncpy(jobs[job_count].command, input_line, MAX_INPUT);
                    jobs[job_count].running = 0; // stopped
                    job_count++;
                    printf("[%d]+ Stopped %s\n", job_count, input_line);
                }
            }

            fg_pid = 0; // reset foreground pid
        }

        if (input_fd != -1) close(input_fd);
        if (output_fd != -1) close(output_fd);
    }
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



void free_paths() {
    for (int i = 0; i < path_count; i++) {
        free(shell_paths[i]);
    }
    path_count = 0;
}

// int pipe(int fds[2])
// fd[0] = fd for read end
// fd[1] = fd for writing 
// 0 = success and -1 = error
// behaves like a queue
// can write 512 bytes at a time 
// read only 1 byte at a time 


// Forward declaration
char *locate_executable(const char *cmd);

void execute_command(char **argv, int input_fd, int output_fd) {
    char *path = locate_executable(argv[0]);
    if (!path) {
        fprintf(stderr, "%s: command not found\n", argv[0]);
        exit(1);
    }

    if (input_fd != -1) { dup2(input_fd, STDIN_FILENO); close(input_fd); }
    if (output_fd != -1) { dup2(output_fd, STDOUT_FILENO); close(output_fd); }

    execv(path, argv);
    perror("exec failed");
    exit(1);
}

void execute_pipeline(char *input_line) {
    char *commands[MAX_CMDS];
    int num_cmds = 0;

    char *tok = strtok(input_line, "|");
    while (tok && num_cmds < MAX_CMDS) {
        while (*tok == ' ') tok++; // trim leading spaces
        char *end = tok + strlen(tok) - 1;
        while (end > tok && (*end == ' ' || *end == '\n')) { *end = '\0'; end--; }
        commands[num_cmds++] = tok;
        tok = strtok(NULL, "|");
    }

    int fds[2*(num_cmds-1)];
    for (int i = 0; i < num_cmds-1; i++)
        if (pipe(fds + i*2) < 0) { perror("pipe"); return; }

    for (int i = 0; i < num_cmds; i++) {
        char *argv[MAX_ARGS];
        int argc = 0;
        tok = strtok(commands[i], " \t");
        while (tok && argc < MAX_ARGS-1) { argv[argc++] = tok; tok = strtok(NULL, " \t"); }
        argv[argc] = NULL;
        if (argc == 0) continue;

        pid_t pid = fork();
        if (pid < 0) { perror("fork"); return; }

        if (pid == 0) {
            if (i != 0) dup2(fds[(i-1)*2], STDIN_FILENO);
            if (i != num_cmds-1) dup2(fds[i*2+1], STDOUT_FILENO);
            for (int j = 0; j < 2*(num_cmds-1); j++) close(fds[j]);
            execute_command(argv, -1, -1);
        }
    }

    for (int i = 0; i < 2*(num_cmds-1); i++) close(fds[i]);
    for (int i = 0; i < num_cmds; i++) wait(NULL);
}

// int num_paths = 0;
char
*locate_executable(const char *cmd) {
    if (strchr(cmd, '/')) {
        if (access(cmd, X_OK) == 0) return strdup(cmd);
        return NULL;
    }

    for (int i = 0; i < path_count; i++) {
        size_t len = strlen(shell_paths[i]) + strlen(cmd) + 2;
        char *full = malloc(len);
        snprintf(full, len, "%s/%s", shell_paths[i], cmd);
        if (access(full, X_OK) == 0) return full;
        free(full);
    }
    return NULL;
}

// signal handling 
void sigint_handler(int sig) {
    if (fg_pid != 0) {
        kill(fg_pid, SIGINT);  
        printf("\n");
    }
}

void sigtstp_handler(int sig) {
    if (fg_pid != 0) {
        kill(fg_pid, SIGTSTP);  
        printf("\n");            
    }
}


void list_jobs() {
    for (int i = 0; i < job_count; i++) {
        printf("[%d] %s %s\n",
            jobs[i].job_id,
            jobs[i].running ? "Running" : "Stopped",
            jobs[i].command);
    }
}

void bring_fg(int job_id) {
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].job_id == job_id) {
            fg_pid = jobs[i].pid;
            jobs[i].running = 1;
            kill(fg_pid, SIGCONT);
            waitpid(fg_pid, NULL, 0);
            fg_pid = 0;
            return;
        }
    }
}

void resume_bg(int job_id) {
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].job_id == job_id) {
            jobs[i].running = 1;
            kill(jobs[i].pid, SIGCONT);
            return;
        }
    }
}

// implementing history
void add_to_history(const char *cmd) {
    if (history_count < MAX_HISTORY) {
        history[history_count++] = strdup(cmd);
    } else {
        free(history[0]);
        for (int i = 1; i < MAX_HISTORY; i++)
            history[i-1] = history[i];
        history[MAX_HISTORY-1] = strdup(cmd);
    }
}

void show_history() {
    for (int i = 0; i < history_count; i++)
        printf("%d  %s\n", i+1, history[i]);
}

void free_history() {
    for (int i = 0; i < history_count; i++)
        free(history[i]);
    history_count = 0;
}
