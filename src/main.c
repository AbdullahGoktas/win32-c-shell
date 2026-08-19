#include "myshell.h"

int main() {
    char *line;
    char **args;
    int shell_active = 1;
    char cwd[1024]; /* Buffer for the current working directory */

    /* REPL: Read, Evaluate, Print, Loop */
    while (shell_active) {
        /* Print dynamic prompt with current working directory */
        if (_getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s> ", cwd);
        } else {
            /* Fallback prompt if _getcwd fails */
            printf("myshell> "); 
        }
        
        line = read_line();
        if (line == NULL) {
            printf("\n");
            break; /* Handle EOF (Ctrl+Z on Windows) */
        }

        /* Add the raw input line to history before parsing */
        /* split_line modifies the string, so it must be recorded beforehand */
        if (line[0] != '\0') {
            add_to_history(line);
        }

        args = split_line(line);
        if (args != NULL) {
            /* Expand environment variables like $PATH or $USER */
            expand_variables(args);
            
            /* Pass the parsed arguments to the logic evaluator */
            execute_logic(args, &shell_active);
            free(args);
        }

        /* Memory Management: Free allocated memory in each iteration */
        free(line);
    }

    /* Clean up history memory allocations before exiting */
    free_history();

    return EXIT_SUCCESS;
}