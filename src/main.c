#include "myshell.h"

int main() {
    char *line;
    char **args;
    int status = 1;

    /* REPL: Read, Evaluate, Print, Loop */
    while (status) {
        printf("myshell> ");
        
        line = read_line();
        args = split_line(line);
        status = execute_command(args);

        /* Memory Management: Free allocated memory in each iteration */
        free(line);
        free(args);
    }

    return EXIT_SUCCESS;
}