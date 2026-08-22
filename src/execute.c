#include "myshell.h"
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

int launch_process(char **args, int is_bg) {
    intptr_t status;

    /* _P_WAIT: Suspend parent process until child finishes */
    /* _P_NOWAIT: Continue executing parent process immediately */
    int mode = is_bg ? _P_NOWAIT : _P_WAIT;
    
    /* _spawnvp: Search PATH for the executable and run it */
    status = _spawnvp(mode, args[0], (const char * const *)args);
    
    if (status == -1) {
        perror("myshell"); 
        return 1; 
    }

    if (is_bg) {
        printf("[Background process started with PID/Handle: %Id]\n", status);
        return 0; 
    }

    return (int)status; 
}

int execute_command(char **args, int *shell_active) {
    if (args[0] == NULL) {
        return 0; 
    }

    int is_bg = 0;
    int i = 0;
    while (args[i] != NULL) {
        i++;
    }
    
    if (i > 0 && strcmp(args[i - 1], "&") == 0) {
        is_bg = 1;
        args[i - 1] = NULL; 
    }

    /* --- I/O Redirection Start --- */
    int saved_stdout = _dup(1);
    int saved_stdin  = _dup(0);
    int j = 0;
    int redirect_err = 0;

    while (args[j] != NULL) {
        if (strcmp(args[j], "<") == 0) {
            if (args[j+1] == NULL) { fprintf(stderr, "myshell: syntax error\n"); redirect_err = 1; break; }
            int fd_in = _open(args[j + 1], _O_RDONLY);
            if (fd_in < 0) { perror("myshell: input file"); redirect_err = 1; break; }
            _dup2(fd_in, 0); 
            _close(fd_in);
        } else if (strcmp(args[j], ">") == 0) {
            if (args[j+1] == NULL) { fprintf(stderr, "myshell: syntax error\n"); redirect_err = 1; break; }
            int fd_out = _open(args[j + 1], _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);
            if (fd_out < 0) { perror("myshell: output file"); redirect_err = 1; break; }
            _dup2(fd_out, 1); 
            _close(fd_out);
        } else if (strcmp(args[j], ">>") == 0) {
            if (args[j+1] == NULL) { fprintf(stderr, "myshell: syntax error\n"); redirect_err = 1; break; }
            int fd_out = _open(args[j + 1], _O_WRONLY | _O_CREAT | _O_APPEND, _S_IREAD | _S_IWRITE);
            if (fd_out < 0) { perror("myshell: append file"); redirect_err = 1; break; }
            _dup2(fd_out, 1);
            _close(fd_out);
        }
        j++;
    }

    j = 0;
    while (args[j] != NULL) {
        if (strcmp(args[j], "<") == 0 || strcmp(args[j], ">") == 0 || strcmp(args[j], ">>") == 0) {
            args[j] = NULL;
            break;
        }
        j++;
    }
    /* --- I/O Redirection End --- */

    int status = 1;

    if (!redirect_err && args[0] != NULL) {
        int builtin_status = execute_builtin(args, shell_active);
        
        if (builtin_status == -1) {
            status = launch_process(args, is_bg);
        } else {
            status = builtin_status;
        }
    }
    
    _dup2(saved_stdout, 1);
    _dup2(saved_stdin, 0);
    _close(saved_stdout);
    _close(saved_stdin);

    return status;
}

/* 
 * execute_pipeline: Splits arguments by '|' and links their standard streams using _pipe() 
 */
int execute_pipeline(char **args, int *shell_active) {
    int i = 0;
    int num_pipes = 0;
    
    /* Count the number of pipes in this segment */
    while (args[i] != NULL) {
        if (strcmp(args[i], "|") == 0) {
            num_pipes++;
        }
        i++;
    }
    
    /* If no pipes, fallback to regular command execution */
    if (num_pipes == 0) {
        return execute_command(args, shell_active);
    }
    
    int saved_stdin = _dup(0);
    int saved_stdout = _dup(1);
    int fd_in = _dup(0); /* The initial input is standard input */
    int status = 0;
    int start = 0;
    
    i = 0;
    while (args[i] != NULL) {
        if (strcmp(args[i], "|") == 0) {
            args[i] = NULL; /* Isolate the current command */
            
            int pipefd[2];
            /* 256 KB buffer size for Windows pipe to prevent deadlocks in synchronous spawn */
            if (_pipe(pipefd, 262144, _O_BINARY) == -1) {
                perror("myshell: pipe error");
                break;
            }
            
            /* Read from previous pipe (or stdin for the first command) */
            _dup2(fd_in, 0);
            _close(fd_in);
            
            /* Write to current pipe */
            _dup2(pipefd[1], 1);
            _close(pipefd[1]);
            
            /* Execute the isolated command */
            status = execute_command(&args[start], shell_active);
            
            /* Save the read end of the pipe for the next command */
            fd_in = pipefd[0]; 
            start = i + 1;
        }
        i++;
    }
    
    /* Process the final command in the pipeline */
    _dup2(fd_in, 0);
    _close(fd_in);
    
    /* Restore standard output for the final command */
    _dup2(saved_stdout, 1);
    
    status = execute_command(&args[start], shell_active);
    
    /* Restore standard input completely */
    _dup2(saved_stdin, 0);
    _close(saved_stdin);
    _close(saved_stdout);
    
    return status;
}

int execute_logic(char **args, int *shell_active) {
    int i = 0;
    int start = 0;
    int status = 0;
    int skip_next = 0;
    
    while (args[i] != NULL) {
        if (strcmp(args[i], "&&") == 0 || strcmp(args[i], "||") == 0) {
            char *operator = args[i];
            args[i] = NULL; 
            
            if (!skip_next) {
                /* Evaluate the pipeline block instead of a single command */
                status = execute_pipeline(&args[start], shell_active);
            }
            
            if (strcmp(operator, "&&") == 0) {
                skip_next = (status != 0); 
            } else if (strcmp(operator, "||") == 0) {
                skip_next = (status == 0); 
            }
            
            start = i + 1; 
        }
        i++;
    }
    
    if (!skip_next && args[start] != NULL) {
        status = execute_pipeline(&args[start], shell_active);
    }
    
    return status;
}