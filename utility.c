/*
 * MYSHELL UTILITY FUNCTIONS
 * Group Members: [Malak Almasri], [Saba Timraz], [Mera Badr alDin]
 * 
 * Description: Utility functions for the shell including command parsing,
 * execution, and built-in command implementations.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "myshell.h"

void init_shell() {
    char path[PATH_MAX];
    if (readlink("/proc/self/exe", path, sizeof(path)) != -1) {
        setenv("shell", path, 1);
    }
}

void print_prompt() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("%s> ", cwd);
    else
        perror("getcwd");
}

void parse_input(char *input, char **args, int *is_background, char **infile, char **outfile, int *append) {
    *is_background = 0;
    *infile = NULL;
    *outfile = NULL;
    *append = 0;

    int i = 0;
    char *token = strtok(input, " \t");
    while (token != NULL && i < MAX_ARGS - 1) {
        if (strcmp(token, "&") == 0) {
            *is_background = 1;
        } else if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " \t");
            if (token) *infile = token;
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " \t");
            if (token) {
                *outfile = token;
                *append = 0;
            }
        } else if (strcmp(token, ">>") == 0) {
            token = strtok(NULL, " \t");
            if (token) {
                *outfile = token;
                *append = 1;
            }
        } else {
            args[i++] = token;
        }
        token = strtok(NULL, " \t");
    }
    args[i] = NULL;
}

int is_internal_command(char *cmd) {
    const char *commands[] = {"cd", "clr", "dir", "environ", "echo", "help", "pause", "quit", NULL};
    for (int i = 0; commands[i]; i++) {
        if (strcmp(cmd, commands[i]) == 0)
            return 1;
    }
    return 0;
}

void execute_internal(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        if (args[1]) {
            if (chdir(args[1]) != 0)
                perror("cd");
        } else {
            char cwd[PATH_MAX];
            getcwd(cwd, sizeof(cwd));
            printf("%s\n", cwd);
        }
    } else if (strcmp(args[0], "clr") == 0) {
        printf("\033[H\033[J");
    } else if (strcmp(args[0], "dir") == 0) {
        char *path = args[1] ? args[1] : ".";
        DIR *d = opendir(path);
        if (d) {
            struct dirent *entry;
            while ((entry = readdir(d)) != NULL)
                printf("%s\n", entry->d_name);
            closedir(d);
        } else {
            perror("dir");
        }
    } else if (strcmp(args[0], "environ") == 0) {
        extern char **environ;
        for (char **env = environ; *env != NULL; env++)
            printf("%s\n", *env);
    } else if (strcmp(args[0], "echo") == 0) {
        for (int i = 1; args[i] != NULL; i++)
            printf("%s ", args[i]);
        printf("\n");
    } else if (strcmp(args[0], "help") == 0) {
        system("more readme");
    } else if (strcmp(args[0], "pause") == 0) {
        printf("Press Enter to continue...");
        while (getchar() != '\n');
    }
}

void execute_command(char **args, int is_background, char *infile, char *outfile, int append) {
    if (args[0] == NULL) return;

    if (strcmp(args[0], "quit") == 0) exit(0);

    if (is_internal_command(args[0])) {
        int out_fd = -1, saved_stdout = -1;
        if (outfile) {
            out_fd = open(outfile, O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), 0644);
            if (out_fd < 0) {
                perror("output redirection");
                return;
            }
            saved_stdout = dup(STDOUT_FILENO);
            dup2(out_fd, STDOUT_FILENO);
        }

        execute_internal(args);

        if (outfile) {
            fflush(stdout);
            dup2(saved_stdout, STDOUT_FILENO);
            close(out_fd);
            close(saved_stdout);
        }
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        setenv("parent", getenv("shell"), 1);

        if (infile) {
            int in_fd = open(infile, O_RDONLY);
            if (in_fd < 0) {
                perror("input redirection");
                exit(1);
            }
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }

        if (outfile) {
            int out_fd = open(outfile, O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), 0644);
            if (out_fd < 0) {
                perror("output redirection");
                exit(1);
            }
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }

        execvp(args[0], args);
        perror("execvp failed");
        exit(1);
    } else if (pid > 0) {
        if (!is_background)
            waitpid(pid, NULL, 0);
    } else {
        perror("fork");
    }
}
