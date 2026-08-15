#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <process.h> /* Windows process management (_spawnvp) */
#include <stdint.h>  /* For intptr_t */

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

/* Parser Functions */
char *read_line(void);
char **split_line(char *line);

/* Execution Functions */
int execute_command(char **args);
int launch_process(char **args);

/* Builtin Functions */
int execute_builtin(char **args);
int lsh_cd(char **args);
int lsh_exit(char **args);

#endif