#include "myshell.h"

/* History Storage */
char *command_history[LSH_HIST_SIZE];
int history_count = 0;

void add_to_history(char *line) {
    /* Do not add empty lines to history */
    if (line == NULL || line[0] == '\0') {
        return;
    }

    /* If history is full, free the oldest command and shift everything left */
    if (history_count >= LSH_HIST_SIZE) {
        free(command_history[0]);
        for (int i = 1; i < LSH_HIST_SIZE; i++) {
            command_history[i - 1] = command_history[i];
        }
        command_history[LSH_HIST_SIZE - 1] = strdup(line);
    } else {
        /* Add new command to the end of the history array */
        command_history[history_count] = strdup(line);
        history_count++;
    }
}

void free_history(void) {
    for (int i = 0; i < history_count; i++) {
        free(command_history[i]);
    }
}

int lsh_history(char **args) {
    (void)args;
    for (int i = 0; i < history_count; i++) {
        printf("  %d  %s\n", i + 1, command_history[i]);
    }
    return 0; /* Success */
}

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
    if (strcmp(args[0], "history") == 0) {
        return lsh_history(args);
    }
    
    /* Builtin not found */
    return -1; 
}