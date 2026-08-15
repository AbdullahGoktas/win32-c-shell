#include "myshell.h"

int lsh_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "myshell: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("myshell");
        }
    }
    return 1;
}

int lsh_exit(char **args) {
    (void)args;
    /* Returning 0 terminates the while loop in main.c */
    return 0; 
}

int execute_builtin(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        return lsh_cd(args);
    }
    if (strcmp(args[0], "exit") == 0) {
        return lsh_exit(args);
    }
    
    /* Builtin not found */
    return -1; 
}