# My Custom Shell

A custom Unix shell written in C, featuring:
- REPL (Read-Eval-Print Loop)
- Basic command execution using `fork` and `execvp`
- Pipelining support (`|`) to chain two commands
- Built-in commands (`cd`, `help`, `exit`)

## Build and Run

```bash
make
./myshell
```

## Usage Examples

- `ls -l`
- `ls -l | grep .c`
- `cd src`
- `help`
- `exit`
