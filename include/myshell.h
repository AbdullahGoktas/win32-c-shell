#ifndef MYSHELL_H
#define MYSHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <process.h> /* Windows process management (_spawnvp) */
#include <direct.h>  /* Windows directory management (_getcwd) */
#include <stdint.h>  /* For intptr_t */

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
#define LSH_HIST_SIZE 100 /* Maximum number of commands in history */

/* Parser Functions */
char *read_line(void);
char **split_line(char *line);
void expand_variables(char **args);

/* Execution Functions */
int execute_logic(char **args, int *shell_active);
int execute_command(char **args, int *shell_active);
int launch_process(char **args, int is_bg);

/* Builtin Functions */
int execute_builtin(char **args, int *shell_active);
int lsh_cd(char **args);
int lsh_exit(char **args, int *shell_active);
int lsh_history(char **args);
int lsh_export(char **args);

/* History Functions */
void add_to_history(char *line);
void free_history(void);

#endif