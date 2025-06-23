
/*
 * MYSHELL HEADER FILE
 * Group Members: [Malak Almasri], [Saba Timraz], [Mera Badr alDin]
 */

#ifndef MYSHELL_H
#define MYSHELL_H

#define MAX_ARGS 100

// Core shell functions
void init_shell();
void print_prompt();
void parse_input(char *input, char **args, int *is_background, char **infile, char **outfile, int *append);
void execute_command(char **args, int is_background, char *infile, char *outfile, int append);

// Internal command functions
int is_internal_command(char *cmd);
void execute_internal(char **args);

#endif
