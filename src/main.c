#include "myshell.h"

int main() {
    char *line;
    char **args;
    int shell_active = 1;

    /* REPL: Read, Evaluate, Print, Loop */
    while (shell_active) {
        printf("myshell> ");
        
        line = read_line();
        if (line == NULL) {
            printf("\n");
            break; /* Handle EOF (Ctrl+Z on Windows) */
        }

        args = split_line(line);
        if (args != NULL) {
            /* Pass the parsed arguments to the logic evaluator */
            execute_logic(args, &shell_active);
            free(args);
        }

        /* Memory Management: Free allocated memory in each iteration */
        free(line);
    }

    return EXIT_SUCCESS;
}