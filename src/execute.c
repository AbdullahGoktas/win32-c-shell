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
    /* Windows'ta shell sürecinin kalıcı olarak yönlendirilmesini önlemek için STDIN ve STDOUT yedeklenir */
    int saved_stdout = _dup(1);
    int saved_stdin  = _dup(0);
    int j = 0;
    int redirect_err = 0;

    while (args[j] != NULL) {
        if (strcmp(args[j], "<") == 0) {
            if (args[j+1] == NULL) { fprintf(stderr, "myshell: syntax error\n"); redirect_err = 1; break; }
            int fd_in = _open(args[j + 1], _O_RDONLY);
            if (fd_in < 0) { perror("myshell: input file"); redirect_err = 1; break; }
            _dup2(fd_in, 0); /* 0: STDIN */
            _close(fd_in);
        } else if (strcmp(args[j], ">") == 0) {
            if (args[j+1] == NULL) { fprintf(stderr, "myshell: syntax error\n"); redirect_err = 1; break; }
            int fd_out = _open(args[j + 1], _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);
            if (fd_out < 0) { perror("myshell: output file"); redirect_err = 1; break; }
            _dup2(fd_out, 1); /* 1: STDOUT */
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

    /* Argüman dizisi, exec çağrısı için ilk yönlendirme operatörünün olduğu yerden kesilir */
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

    /* Eğer yönlendirme hatası yoksa komutu çalıştır */
    if (!redirect_err && args[0] != NULL) {
        int builtin_status = execute_builtin(args, shell_active);
        
        if (builtin_status == -1) {
            status = launch_process(args, is_bg);
        } else {
            status = builtin_status;
        }
    }
    
    /* Komut (built-in veya sistem komutu) bittikten sonra standart akışları geri yükle */
    _dup2(saved_stdout, 1);
    _dup2(saved_stdin, 0);
    _close(saved_stdout);
    _close(saved_stdin);

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
                status = execute_command(&args[start], shell_active);
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
        status = execute_command(&args[start], shell_active);
    }
    
    return status;
}