#include "shell.h"

int launch_process(char **args) {
    intptr_t status;

    /* _P_WAIT: Suspend parent process until child finishes */
    /* _spawnvp: Search PATH for the executable and run it */
    
    status = _spawnvp(_P_WAIT, args[0], (const char * const *)args);
    
    if (status == -1) {
        /* Print error if command execution fails */
        perror("myshell"); 
    }

    /* Continue the REPL loop */
    return 1; 
}

int execute_command(char **args) {
    if (args[0] == NULL) {
        /* Empty command entered, continue */
        return 1; 
    }

    /* Check if the command is a builtin (cd, exit) */
    int builtin_status = execute_builtin(args);
    
    /* If -1 is returned, it is a standard system command */
    if (builtin_status == -1) {
        return launch_process(args);
    }
    
    return builtin_status;
}