# hfsh26 - Unix Shell Implementation

A fully-functional Unix shell written in C++ that demonstrates deep understanding of operating systems concepts, process management, and system programming.

## 🎯 Project Overview

**hfsh26** (Horned Frog Shell v26) is a custom command-line interface that replicates core functionality of Unix shells like bash. This project showcases proficiency in low-level systems programming, process control, and inter-process communication.

Built as part of an Operating Systems course, this shell handles command execution, I/O redirection, parallel processing, and path management—all from scratch without using high-level system abstractions.

## ✨ Key Features

### Core Functionality
- **Interactive & Batch Modes**: Supports both real-time command input and script execution from files
- **Process Management**: Implements fork-exec pattern for creating and managing child processes
- **Command Path Resolution**: Custom path variable system for executable lookup across multiple directories
- **I/O Redirection**: Full support for stdout/stderr redirection to files using the `>` operator
- **Parallel Command Execution**: Execute multiple commands concurrently using the `&` operator
- **Built-in Commands**: Native implementations of `exit`, `cd`, and `path` commands

### Technical Highlights
- **Memory Management**: Proper allocation and deallocation of dynamic command structures
- **Error Handling**: Robust error detection and reporting for invalid commands and system call failures
- **Tokenization**: Custom lexical analysis for parsing complex command-line inputs
- **File Descriptor Management**: Careful handling of stdin/stdout/stderr redirection

## 🛠️ Technical Implementation

### Process Control
```cpp
// Fork-exec pattern for command execution
pid_t pid = fork();
if (pid == 0) {
    // Child process: execute command
    execv(executable.c_str(), toks);
    exit(1);
} else {
    // Parent process: wait for completion
    wait(NULL);
}
```

### Parallel Execution
Commands separated by `&` are executed concurrently:
```bash
hfsh26> cmd1 & cmd2 args & cmd3
# All three commands run simultaneously
```

Implementation forks all processes first, then waits for completion:
```cpp
// Fork all commands
for (each command) {
    pid_t pid = fork();
    if (pid == 0) execute_command();
    else pids.push_back(pid);
}
// Wait for all to complete
for (each pid) waitpid(pid, NULL, 0);
```

### I/O Redirection
Redirects both stdout and stderr to specified file:
```cpp
int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
dup2(fd, STDOUT_FILENO);
dup2(fd, STDERR_FILENO);
```

## 📋 System Calls Utilized

| System Call | Purpose |
|------------|---------|
| `fork()` | Create child processes |
| `execv()` | Execute programs |
| `wait()`/`waitpid()` | Synchronize parent-child processes |
| `chdir()` | Change working directory |
| `access()` | Check file existence and permissions |
| `open()` | File operations for redirection |
| `dup2()` | Duplicate file descriptors |
| `write()` | Error message output |

## 🏗️ Architecture

### Code Structure
```
hfsh26/
├── hfsh26.cpp          # Main shell implementation
├── parse.l             # Lexical analyzer (provided)
├── Makefile            # Build configuration
└── tests/              # Test cases
```

### Key Components

**Command Processing Pipeline:**
1. **Input Parsing** (`getToksArguments`) - Tokenize command line
2. **Parallel Detection** (`parseLine`) - Split on `&` operators
3. **Command Classification** (`processCmd`) - Route to built-ins or external programs
4. **Execution** (`handleJob`) - Path resolution, fork-exec, redirection

**Built-in Commands:**
- `exit` - Clean shell termination
- `cd <directory>` - Directory navigation
- `path <dir1> <dir2> ...` - Modify executable search path

## 🚀 Usage

### Interactive Mode
```bash
$ ./hfsh26
hfsh26> ls -la
hfsh26> cd /tmp
hfsh26> path /bin /usr/bin
hfsh26> exit
```

### Batch Mode
```bash
$ ./hfsh26 script.txt
# Executes commands from script.txt
```

### Example Commands
```bash
# Basic command execution
hfsh26> ls -l /home

# I/O Redirection
hfsh26> grep pattern file.txt > results.txt

# Parallel execution
hfsh26> sleep 3 & sleep 2 & echo "done"

# Path management
hfsh26> path /bin /usr/bin /usr/local/bin
```

## 🔧 Building & Testing

### Compilation
```bash
make
```

## 💡 Technical Challenges Solved

1. **Race Conditions**: Proper synchronization between parent and child processes using wait semantics
2. **Memory Management**: Dynamic allocation of command structures with proper cleanup
3. **Edge Cases**: Handling empty commands, malformed input, and invalid paths
4. **File Descriptor Management**: Correct redirection without file descriptor leaks
5. **Concurrent Execution**: Managing multiple child processes without deadlocks

## 🎓 Learning Outcomes

- **Systems Programming**: Deep understanding of Unix process model
- **C/C++ Proficiency**: Mixture of C system calls and C++ STL (vectors, strings)
- **Debugging Skills**: Extensive use of gdb and systematic troubleshooting
- **Software Design**: Modular architecture with clear separation of concerns
- **Testing**: Comprehensive test-driven development approach

## 📊 Project Metrics

- **Lines of Code**: ~350 LOC (excluding provided lexer)
- **System Calls**: 10+ different Unix system calls
- **Test Coverage**: 22 comprehensive test cases
- **Error Handling**: Unified error reporting mechanism

## 🔍 Code Quality

- **Modular Design**: Functions averaging 20-30 lines with single responsibilities
- **Documentation**: Comprehensive inline comments and function headers
- **Error Handling**: Graceful degradation with informative error messages
- **Memory Safety**: No memory leaks (verified with valgrind)

## 📝 Technical Specifications

- **Language**: C++ (C++11 compatible)
- **Platform**: Linux (Ubuntu 24.04 LTS)
- **Compiler**: g++ with standard flags
- **Dependencies**: Standard library + POSIX system calls

## 🏆 Skills Demonstrated

**Operating Systems:**
- Process creation and management
- Inter-process communication
- File I/O and redirection
- Signal handling

**Software Engineering:**
- Clean code principles
- Modular architecture
- Version control (Git)
- Test-driven development

**Programming:**
- Low-level systems programming
- C/C++ expertise
- Memory management
- Debugging and troubleshooting

---

**Author**: Griffin Kuchar  
**Course**: Operating Systems  
**Institution**: Texas Christian University  
**Date**: February 2026

*This project demonstrates production-level systems programming skills and deep understanding of Unix internals.*