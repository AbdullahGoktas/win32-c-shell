#include "myshell.h"

int launch_process(char **args, int is_bg) {
    intptr_t status;

    /* _P_WAIT: Suspend parent process until child finishes */
    /* _P_NOWAIT: Continue executing parent process immediately */
    int mode = is_bg ? _P_NOWAIT : _P_WAIT;
    
    /* _spawnvp: Search PATH for the executable and run it */
    status = _spawnvp(mode, args[0], (const char * const *)args);
    
    if (status == -1) {
        /* Print error if command execution fails */
        perror("myshell"); 
        return 1; /* Return failure code */
    }

    if (is_bg) {
        printf("[Background process started with PID/Handle: %Id]\n", status);
        return 0; /* Background tasks immediately return success to the shell */
    }

    /* Return actual exit status of the command */
    return (int)status; 
}

int execute_command(char **args, int *shell_active) {
    if (args[0] == NULL) {
        /* Empty command entered, continue */
        return 0; 
    }

    /* Check for background execution '&' */
    int is_bg = 0;
    int i = 0;
    while (args[i] != NULL) {
        i++;
    }
    
    /* If the last argument is '&', enable background mode */
    if (i > 0 && strcmp(args[i - 1], "&") == 0) {
        is_bg = 1;
        args[i - 1] = NULL; /* Remove '&' from arguments before execution */
    }

    /* Check if the command is a builtin (cd, exit) */
    int builtin_status = execute_builtin(args, shell_active);
    
    /* If -1 is returned, it is a standard system command */
    if (builtin_status == -1) {
        return launch_process(args, is_bg);
    }
    
    return builtin_status;
}

int execute_logic(char **args, int *shell_active) {
    int i = 0;
    int start = 0;
    int status = 0;
    int skip_next = 0;
    
    while (args[i] != NULL) {
        if (strcmp(args[i], "&&") == 0 || strcmp(args[i], "||") == 0) {
            char *operator = args[i];
            args[i] = NULL; /* Isolate the current command */
            
            if (!skip_next) {
                status = execute_command(&args[start], shell_active);
            }
            
            /* Evaluate logical condition for the NEXT command */
            if (strcmp(operator, "&&") == 0) {
                /* AND: skip next if current failed (status != 0) */
                skip_next = (status != 0); 
            } else if (strcmp(operator, "||") == 0) {
                /* OR: skip next if current succeeded (status == 0) */
                skip_next = (status == 0); 
            }
            
            start = i + 1; /* Move start pointer to the next command */
        }
        i++;
    }
    
    /* Execute the final or only command in the chain */
    if (!skip_next && args[start] != NULL) {
        status = execute_command(&args[start], shell_active);
    }
    
    return status;
}