# cpp-shell

A small interactive Unix-style shell written in C++20. It can run external programs, change directories, use pipelines and redirection, and start background jobs.

## Features

- Built-in `cd`, `help`, `checkbg`, and `exit` commands
- External command execution with arguments
- Pipes, for example: `ls | grep cpp`
- Output redirection: `ls > files.txt` and `ls >> files.txt`
- Background jobs: `sleep 10 &`
- Quoted arguments, such as `echo "hello world"`
- Graceful EOF handling

## Requirements

This project uses POSIX APIs (`fork`, `pipe`, `dup2`, and `waitpid`). Build it in:

- Linux or WSL
- macOS
- MSYS2/MinGW64

A C++20 compiler is required (`g++`, `clang++`, or `c++`).

## Build and run

### Linux, macOS, WSL, or MSYS2

```sh
make
./cpp-shell
```

On systems without `make`, compile directly:

```sh
c++ -std=c++20 -Wall -Wextra -Wpedantic \
  Project1.cpp cd.cpp execute.cpp inputParser.cpp pipes.cpp redirections.cpp \
  -o cpp-shell
./cpp-shell
```

### Visual Studio

Open `Project1.slnx` and build the project using a POSIX-capable environment. The source is not intended for native MSVC builds because it depends on POSIX process APIs. For native Windows builds, use MSYS2/MinGW64 or build under WSL.

## Usage examples

```text
[~ / cpp-shell]$ help
[~ / cpp-shell]$ cd ~
[~ / cpp-shell]$ ls -l
[~ / cpp-shell]$ ls | grep cpp
[~ / cpp-shell]$ ls > files.txt
[~ / cpp-shell]$ sleep 5 &
[~ / cpp-shell]$ checkbg
[~ / cpp-shell]$ exit
```

## Project files

- `Project1.cpp` — main loop and built-in commands
- `cd.cpp` — directory changes
- `execute.cpp` — process creation and background jobs
- `inputParser.cpp` — command tokenization
- `pipes.cpp` — pipeline handling
- `redirections.cpp` — output file redirection
