# My Custom Shell

A comprehensive custom shell written in C for Windows environments. It bridges the gap between Unix-like shell features and Windows process management, providing a robust command-line interface.

## Features

- **REPL & Dynamic Prompt:** Read-Eval-Print Loop with a dynamic prompt showing the current working directory (`_getcwd`).
- **Windows Process Execution:** Utilizes `<process.h>` and `_spawnvp` for native Windows command execution.
- **Pipelining (`|`):** Connects the output of one command to the input of another using anonymous Windows pipes (`_pipe`).
- **I/O Redirection (`>`, `<`, `>>`):** Redirects standard input and output streams to files.
- **Logical Operators (`&&`, `||`):** Chains commands conditionally based on the exit status of the previous process.
- **Background Execution (`&`):** Executes processes non-blockingly (`_P_NOWAIT`), returning control to the shell immediately.
- **Command History:** Stores the last 100 executed commands in memory, accessible via the `history` built-in.
- **Environment Variables:** Expands variables (e.g., `$PATH`, `$USER`) and allows setting them via the `export` command.
- **Script Execution:** Supports batch execution by passing a script file as an argument (e.g., `./myshell.exe script.txt`).
- **Built-in Commands:** `cd`, `history`, `export`, and `exit`.

## Build and Run

Ensure you have `gcc` and `make` installed in your environment (e.g., MinGW or MSYS2).

```bash
# To compile the project:
make

# To run in interactive mode (REPL):
./myshell.exe

# To run a batch script:
./myshell.exe my_script.txt