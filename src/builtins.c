#include "myshell.h"

int lsh_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "myshell: expected argument to \"cd\"\n");
        return 1; /* Return 1 (error code) for failure */
    } else {
        if (chdir(args[1]) != 0) {
            perror("myshell");
            return 1; /* Return 1 (error code) for failure */
        }
    }
    return 0; /* Return 0 for success */
}

int lsh_exit(char **args, int *shell_active) {
    (void)args;
    *shell_active = 0; /* Terminates the while loop in main.c */
    return 0; /* Success */
}

int execute_builtin(char **args, int *shell_active) {
    if (strcmp(args[0], "cd") == 0) {
        return lsh_cd(args);
    }
    if (strcmp(args[0], "exit") == 0) {
        return lsh_exit(args, shell_active);
    }
    
    /* Builtin not found */
    return -1; 
}