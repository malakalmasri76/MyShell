/*
 * MYSHELL - Simple Shell Implementation
 * 
 * Group Members: [Malak Almasri], [Saba Timraz], [Mera Badr alDin]
 * 
 * Description: A simple shell program that supports basic command execution,
 * I/O redirection, background processes, and built-in commands.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include "myshell.h"

#define MAX_INPUT 1024
#define MAX_ARGS 100

int main(int argc, char *argv[]) {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];
    char *infile, *outfile;
    int is_background, append;
    FILE *input_source = stdin;

    init_shell();

    if (argc == 2) {
        input_source = fopen(argv[1], "r");
        if (!input_source) {
            perror("Batch file error");
            return 1;
        }
    }

    while (1) {
        if (input_source == stdin)
            print_prompt();

        if (fgets(input, MAX_INPUT, input_source) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) == 0)
            continue;

        parse_input(input, args, &is_background, &infile, &outfile, &append);
        execute_command(args, is_background, infile, outfile, append);
    }

    if (input_source != stdin)
        fclose(input_source);

    return 0;
}