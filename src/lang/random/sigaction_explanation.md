# Understanding `struct sigaction` in Linux

## Overview

The `struct sigaction` is a POSIX structure used to configure how signals are handled in Linux/UNIX systems. It provides much more control and flexibility compared to the older `signal()` function.

## Structure Definition

```c
struct sigaction {
    void (*sa_handler)(int);           // Simple signal handler function
    void (*sa_sigaction)(int, siginfo_t *, void *);  // Advanced signal handler with context
    sigset_t sa_mask;                  // Signals to block during handler execution
    int sa_flags;                      // Flags to modify signal behavior
    void (*sa_restorer)(void);         // Obsolete, should not be used
};
```

## Field-by-Field Explanation

### 1. `sa_handler` vs `sa_sigaction`

These are **mutually exclusive** - you use one OR the other, never both:

#### `sa_handler` - Simple Handler
```c
void (*sa_handler)(int signum);
```
- Takes only the signal number as parameter
- Used for basic signal handling
- Compatible with older `signal()` function style

#### `sa_sigaction` - Advanced Handler  
```c
void (*sa_sigaction)(int signum, siginfo_t *info, void *context);
```
- Takes signal number, detailed info, and context
- Provides much more information about the signal
- Must set `SA_SIGINFO` flag when using this

**Special Values:**
- `SIG_DFL`: Use default signal action
- `SIG_IGN`: Ignore the signal

### 2. `sa_mask` - Signal Blocking During Handler

```c
sigset_t sa_mask;
```

This controls which signals are **temporarily blocked** while your signal handler is executing:

- Prevents signal handler from being interrupted by other signals
- The signal being handled is automatically blocked (unless `SA_NODEFER` is set)
- Use `sigemptyset()`, `sigfillset()`, `sigaddset()`, `sigdelset()` to manipulate

### 3. `sa_flags` - Behavior Modification Flags

#### Most Important Flags:

**`SA_RESTART`**
- Automatically restart interrupted system calls
- Without this: system calls return `EINTR` when interrupted by signals
- With this: system calls are automatically restarted after signal handler completes

**`SA_SIGINFO`** 
- Required when using `sa_sigaction` instead of `sa_handler`
- Enables extended signal information

**`SA_NODEFER`**
- Don't automatically block the signal being handled
- Allows recursive signal handling (dangerous!)

**`SA_RESETHAND`**
- Reset signal handler to `SIG_DFL` after one execution
- Equivalent to old `signal()` behavior

**`SA_NOCLDSTOP`**
- For `SIGCHLD`: don't generate signal when child stops (only when it terminates)

**`SA_NOCLDWAIT`**
- For `SIGCHLD`: don't create zombie processes

## Complete Example Analysis

Let's analyze the sigaction setup from our test program:

```cpp
struct sigaction sa;

// Standard signal handler setup
sa.sa_handler = signal_handler;    // Use simple handler function
sigemptyset(&sa.sa_mask);         // Don't block any additional signals
sa.sa_flags = SA_RESTART;         // Restart interrupted system calls

// Install the handler
sigaction(SIGINT, &sa, nullptr);
```

### What this does:
1. **Handler**: When `SIGINT` arrives, call `signal_handler(int signum)`
2. **Mask**: Don't block any other signals during handler execution
3. **Flags**: If a system call is interrupted by this signal, restart it automatically
4. **Installation**: Apply this configuration to `SIGINT`

## Advanced Example with `SA_SIGINFO`

```cpp
void advanced_handler(int signum, siginfo_t *info, void *context) {
    printf("Signal %d from PID %d\n", signum, info->si_pid);
    printf("Signal sent by: ");
    switch(info->si_code) {
        case SI_USER:    printf("kill() or raise()\n"); break;
        case SI_QUEUE:   printf("sigqueue()\n"); break;
        case SI_TIMER:   printf("timer expiration\n"); break;
        case SI_ASYNCIO: printf("async I/O completion\n"); break;
        default:         printf("other source\n"); break;
    }
}

struct sigaction sa;
sa.sa_sigaction = advanced_handler;  // Use advanced handler
sigemptyset(&sa.sa_mask);
sigaddset(&sa.sa_mask, SIGUSR2);     // Block SIGUSR2 during handler
sa.sa_flags = SA_SIGINFO | SA_RESTART;  // Enable siginfo + restart

sigaction(SIGUSR1, &sa, nullptr);
```

## Signal Masking Deep Dive

The `sa_mask` field is crucial for preventing race conditions:

```cpp
struct sigaction sa;
sa.sa_handler = my_handler;
sigemptyset(&sa.sa_mask);

// Block SIGUSR2 while handling SIGUSR1
sigaddset(&sa.sa_mask, SIGUSR2);  

sa.sa_flags = SA_RESTART;
sigaction(SIGUSR1, &sa, nullptr);
```

**What happens when SIGUSR1 arrives:**
1. Current signal mask is saved
2. `SIGUSR1` is automatically added to blocked signals
3. `SIGUSR2` is also blocked (because it's in `sa_mask`)
4. Handler executes
5. Original signal mask is restored
6. Blocked signals may now be delivered

## Common Pitfalls and Best Practices

### ❌ Don't Do This:
```cpp
// Unsafe - no signal blocking, can cause race conditions
struct sigaction sa;
sa.sa_handler = unsafe_handler;
sigemptyset(&sa.sa_mask);  // No additional blocking
sa.sa_flags = 0;           // No SA_RESTART
```

### ✅ Better Approach:
```cpp
// Safe - proper signal blocking and restart behavior
struct sigaction sa;
sa.sa_handler = safe_handler;
sigemptyset(&sa.sa_mask);
sigaddset(&sa.sa_mask, SIGTERM);  // Block related signals
sigaddset(&sa.sa_mask, SIGUSR1);
sa.sa_flags = SA_RESTART;         // Restart interrupted calls
```

## Process vs Thread Signal Delivery

**Key Point**: In multi-threaded programs, signals are delivered to the **process**, but handled by **any thread** that doesn't have the signal blocked.

### Thread-Specific Signal Masking:
```cpp
// Block signals in specific threads
sigset_t set;
sigemptyset(&set);
sigaddset(&set, SIGUSR1);
pthread_sigmask(SIG_BLOCK, &set, nullptr);  // This thread won't receive SIGUSR1
```

### Dedicated Signal Thread Pattern:
```cpp
// Block signals in all threads except signal handler thread
sigset_t set;
sigfillset(&set);
pthread_sigmask(SIG_BLOCK, &set, nullptr);

// Create dedicated signal handling thread
std::thread signal_thread([]() {
    sigset_t wait_set;
    sigemptyset(&wait_set);
    sigaddset(&wait_set, SIGINT);
    sigaddset(&wait_set, SIGTERM);
    
    int sig;
    while (true) {
        sigwait(&wait_set, &sig);  // Wait for signals
        handle_signal(sig);
    }
});
```

## System Call Interruption

Without `SA_RESTART`, system calls can be interrupted:

```cpp
// This read() might return -1 with errno = EINTR
ssize_t result = read(fd, buffer, size);
if (result == -1 && errno == EINTR) {
    // Signal interrupted the read - need to retry
}
```

With `SA_RESTART`, the system automatically retries the `read()` after the signal handler completes.

## Summary

`struct sigaction` provides:
1. **Flexible handler specification** (`sa_handler` vs `sa_sigaction`)
2. **Signal masking control** (`sa_mask`) for race condition prevention
3. **Behavior modification** (`sa_flags`) for system call handling and more
4. **Thread-aware signal handling** in multi-threaded applications

It's the modern, robust way to handle signals in POSIX systems, replacing the older and less reliable `signal()` function.
