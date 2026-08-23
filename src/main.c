#include "myshell.h"

int main(int argc, char **argv) {
    FILE *input_stream = stdin;
    int interactive = 1;

    /* Check if a script file was passed as an argument */
    if (argc > 1) {
        input_stream = fopen(argv[1], "r");
        if (!input_stream) {
            fprintf(stderr, "myshell: Cannot open script file %s\n", argv[1]);
            return EXIT_FAILURE;
        }
        interactive = 0; /* Disable dynamic prompt in script mode */
    }

    char *line;
    char **args;
    int shell_active = 1;
    char cwd[1024];

    /* REPL / Script Execution Loop */
    while (shell_active) {
        
        /* Print dynamic prompt only in interactive mode */
        if (interactive) {
            if (_getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s> ", cwd);
            } else {
                printf("myshell> "); 
            }
        }
        
        /* Read from stdin or file */
        line = read_line(input_stream);
        
        if (line == NULL) {
            if (interactive) {
                printf("\n");
            }
            break; /* EOF reached (Ctrl+Z or end of script file) */
        }

        /* Add the raw input line to history before parsing */
        if (line[0] != '\0' && interactive) {
            add_to_history(line);
        }

        args = split_line(line);
        if (args != NULL && args[0] != NULL) {
            /* Expand environment variables like $PATH or $USER */
            expand_variables(args);
            
            /* Expand wildcards like *.txt or ?est.c */
            args = expand_wildcards(args);
            
            /* Pass the parsed arguments to the logic evaluator */
            execute_logic(args, &shell_active);
        }
        
        if (args != NULL) {
            free(args);
        }

        /* Memory Management: Free allocated memory in each iteration */
        free(line);
    }

    /* Clean up resources */
    if (!interactive) {
        fclose(input_stream);
    }
    
    free_history();

    return EXIT_SUCCESS;
}