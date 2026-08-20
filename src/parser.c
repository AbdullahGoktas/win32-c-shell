#include "myshell.h"

char *read_line(FILE *stream) {
    int bufsize = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "myshell: memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        /* Read a character from the provided stream (stdin or file) */
        c = fgetc(stream);

        /* If we hit EOF, handle appropriately */
        if (c == EOF) {
            free(buffer);
            return NULL; /* Send NULL to main to exit shell or finish script */
        }
        
        /* If we hit newline, terminate string and return */
        if (c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        /* If we have exceeded the buffer, reallocate */
        if (position >= bufsize) {
            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr, "myshell: memory allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **split_line(char *line) {
    int bufsize = LSH_TOK_BUFSIZE;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "myshell: memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "myshell: memory allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, LSH_TOK_DELIM);
    }
    
    /* Null-terminate the array of tokens (required by process functions) */
    tokens[position] = NULL; 
    return tokens;
}

void expand_variables(char **args) {
    int i = 0;
    while (args[i] != NULL) {
        /* Check if the argument starts with '$' indicating a variable */
        if (args[i][0] == '$' && strlen(args[i]) > 1) {
            char *env_name = args[i] + 1; /* Skip the '$' character */
            char *env_val = getenv(env_name);
            
            if (env_val != NULL) {
                /* Replace the token with the environment variable value */
                args[i] = env_val; 
            } else {
                /* If the variable is not found, replace with an empty string */
                args[i] = ""; 
            }
        }
        i++;
    }
}