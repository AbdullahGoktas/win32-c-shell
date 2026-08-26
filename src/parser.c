#include "myshell.h"
#include <windows.h> /* Required for FindFirstFile and FindNextFile */
#include <conio.h>   /* Required for _getch() */

char *read_line(FILE *stream) {
    int bufsize = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "myshell: memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    /* Script mode: standard line-buffered reading */
    if (stream != stdin) {
        while (1) {
            c = fgetc(stream);
            if (c == EOF) {
                free(buffer);
                return NULL;
            }
            if (c == '\n') {
                buffer[position] = '\0';
                return buffer;
            }
            buffer[position++] = c;

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

    /* Interactive mode: character-by-character reading for Tab completion */
    while (1) {
        c = _getch();

        /* Ctrl+Z (EOF in Windows) */
        if (c == 26 || c == EOF) { 
            free(buffer);
            return NULL;
        }

        /* Enter key */
        if (c == '\r' || c == '\n') { 
            printf("\n");
            buffer[position] = '\0';
            return buffer;
        }

        /* Backspace handling */
        if (c == '\b') { 
            if (position > 0) {
                position--;
                printf("\b \b"); /* Move cursor back, print space, move back again */
            }
            continue;
        }

        /* Tab completion handling */
        if (c == '\t') { 
            buffer[position] = '\0';
            
            /* Find the start of the current word being typed */
            int word_start = position;
            while (word_start > 0 && buffer[word_start - 1] != ' ') {
                word_start--;
            }

            if (word_start == position) {
                continue; /* No word to complete */
            }

            char *prefix = &buffer[word_start];
            char search_pattern[MAX_PATH];
            snprintf(search_pattern, MAX_PATH, "%s*", prefix);

            WIN32_FIND_DATAA findData;
            HANDLE hFind = FindFirstFileA(search_pattern, &findData);

            if (hFind != INVALID_HANDLE_VALUE) {
                /* Ignore "." and ".." files */
                while (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) {
                    if (FindNextFileA(hFind, &findData) == 0) {
                        break;
                    }
                }

                if (strncmp(findData.cFileName, prefix, strlen(prefix)) == 0) {
                    char *completion = findData.cFileName + strlen(prefix);
                    
                    /* Print the completed part to the console */
                    printf("%s", completion);
                    
                    /* Append the completed part to the buffer */
                    for (int i = 0; completion[i] != '\0'; i++) {
                        buffer[position++] = completion[i];
                        if (position >= bufsize - 1) {
                            bufsize += LSH_RL_BUFSIZE;
                            buffer = realloc(buffer, bufsize);
                            if (!buffer) {
                                fprintf(stderr, "myshell: memory allocation error\n");
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                }
                FindClose(hFind);
            }
            continue;
        }

        /* Standard character input */
        putchar(c);
        buffer[position++] = c;

        if (position >= bufsize - 1) {
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
    
    tokens[position] = NULL; 
    return tokens;
}

void expand_variables(char **args) {
    int i = 0;
    while (args[i] != NULL) {
        if (args[i][0] == '$' && strlen(args[i]) > 1) {
            char *env_name = args[i] + 1;
            char *env_val = getenv(env_name);
            
            if (env_val != NULL) {
                args[i] = env_val; 
            } else {
                args[i] = ""; 
            }
        }
        i++;
    }
}

char **expand_wildcards(char **args) {
    int bufsize = LSH_TOK_BUFSIZE;
    int position = 0;
    char **new_args = malloc(bufsize * sizeof(char*));

    if (!new_args) {
        fprintf(stderr, "myshell: memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; args[i] != NULL; i++) {
        if (strchr(args[i], '*') != NULL || strchr(args[i], '?') != NULL) {
            WIN32_FIND_DATAA findFileData;
            HANDLE hFind = FindFirstFileA(args[i], &findFileData);

            if (hFind == INVALID_HANDLE_VALUE) {
                new_args[position++] = args[i];
                if (position >= bufsize) {
                    bufsize += LSH_TOK_BUFSIZE;
                    new_args = realloc(new_args, bufsize * sizeof(char*));
                }
            } else {
                do {
                    if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
                        new_args[position++] = strdup(findFileData.cFileName);
                        if (position >= bufsize) {
                            bufsize += LSH_TOK_BUFSIZE;
                            new_args = realloc(new_args, bufsize * sizeof(char*));
                        }
                    }
                } while (FindNextFileA(hFind, &findFileData) != 0);
                FindClose(hFind);
            }
        } else {
            new_args[position++] = args[i];
            if (position >= bufsize) {
                bufsize += LSH_TOK_BUFSIZE;
                new_args = realloc(new_args, bufsize * sizeof(char*));
            }
        }
    }
    
    new_args[position] = NULL;
    free(args);
    return new_args;
}