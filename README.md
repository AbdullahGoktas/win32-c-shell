# My Custom Shell

A custom shell written in C for Windows environments, featuring:
- REPL (Read-Eval-Print Loop)
- Command execution using Windows process management (`_spawnvp`)
- Logical operators for command chaining (`&&`, `||`)
- Background process execution (`&`)
- Built-in commands (`cd`, `exit`)

## Build and Run

Ensure you have `gcc` and `make` installed in your environment (e.g., MinGW or MSYS2).

```bash
make
./myshell.exe