#define _GNU_SOURCE
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <cstring>
#include <errno.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/types.h>

/// Global variables for signal handling
std::atomic<bool> keep_running{true};
std::atomic<int> signal_count{0};

/// Thread-safe signal handler
void signal_handler(int signum) {
    signal_count++;
    
    /// Get thread ID
    pid_t tid = syscall(SYS_gettid);
    pid_t pid = getpid();
    
    std::cout << "[PID: " << pid << ", TID: " << tid << "] "
              << "Caught signal " << signum << " (" << strsignal(signum) << ")" 
              << " - Count: " << signal_count.load() << std::endl;
    
    /// Handle specific signals
    switch(signum) {
        case SIGINT:
            std::cout << "SIGINT received - Graceful shutdown initiated" << std::endl;
            keep_running = false;
            break;
        case SIGTERM:
            std::cout << "SIGTERM received - Termination requested" << std::endl;
            keep_running = false;
            break;
        case SIGUSR1:
            std::cout << "SIGUSR1 received - User defined signal 1" << std::endl;
            break;
        case SIGUSR2:
            std::cout << "SIGUSR2 received - User defined signal 2" << std::endl;
            break;
        case SIGALRM:
            std::cout << "SIGALRM received - Timer expired" << std::endl;
            break;
        case SIGCHLD:
            std::cout << "SIGCHLD received - Child process state changed" << std::endl;
            break;
        case SIGPIPE:
            std::cout << "SIGPIPE received - Broken pipe" << std::endl;
            break;
        case SIGHUP:
            std::cout << "SIGHUP received - Hangup detected" << std::endl;
            break;
        default:
            std::cout << "Unknown signal received" << std::endl;
            break;
    }
}

/// Worker thread function
void worker_thread(int thread_id) {
    pid_t tid = syscall(SYS_gettid);
    std::cout << "Worker thread " << thread_id << " started with TID: " << tid << std::endl;
    
    /// Each thread does some work
    int work_counter = 0;
    while (keep_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        work_counter++;
        
        if (work_counter % 5 == 0) {
            std::cout << "[Thread " << thread_id << ", TID: " << tid << "] "
                      << "Working... iteration " << work_counter << std::endl;
        }
        
        /// Thread 1 sends signals to demonstrate signal delivery
        if (thread_id == 1 && work_counter % 10 == 0) {
            std::cout << "[Thread " << thread_id << "] Sending SIGUSR1 to process" << std::endl;
            kill(getpid(), SIGUSR1);
        }
    }
    
    std::cout << "Worker thread " << thread_id << " (TID: " << tid << ") exiting" << std::endl;
}

/// Function to setup signal handlers
void setup_signal_handlers() {
    struct sigaction sa;
    
    /// Standard signal handler setup
    /// sa_handler: pointer to our signal handling function
    /// This is the "simple" handler that only receives the signal number
    sa.sa_handler = signal_handler;
    
    /// sa_mask: set of signals to block while our handler is executing
    /// sigemptyset() initializes it to block no additional signals
    /// The signal being handled (e.g., SIGINT) is automatically blocked
    sigemptyset(&sa.sa_mask);
    
    /// sa_flags: behavior modification flags
    /// SA_RESTART: automatically restart interrupted system calls
    /// Without this, system calls would return EINTR when interrupted by signals
    sa.sa_flags = SA_RESTART; /// Restart interrupted system calls
    
    /// Install handlers for various signals
    std::vector<int> signals_to_catch = {
        SIGINT,    /// Interrupt (Ctrl+C)
        SIGTERM,   /// Termination request
        SIGUSR1,   /// User-defined signal 1
        SIGUSR2,   /// User-defined signal 2
        SIGALRM,   /// Timer signal
        SIGCHLD,   /// Child status changed
        SIGPIPE,   /// Broken pipe
        SIGHUP     /// Hangup
    };
    
    /// Install the signal handler for each signal
    /// sigaction(signal_number, new_action, old_action)
    /// - signal_number: which signal to handle
    /// - new_action: pointer to our sigaction struct
    /// - old_action: can store previous handler (we use nullptr)
    for (int sig : signals_to_catch) {
        if (sigaction(sig, &sa, nullptr) == -1) {
            perror("sigaction");
            std::cout << "Failed to install handler for signal " << sig << std::endl;
        } else {
            std::cout << "Handler installed for signal " << sig 
                      << " (" << strsignal(sig) << ")" << std::endl;
        }
    }
}

/// Function to demonstrate signal masking in threads
void demonstrate_signal_masking() {
    std::cout << "\n=== Demonstrating Signal Masking ===" << std::endl;
    
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR2);
    
    /// Block SIGUSR2 in current thread
    if (pthread_sigmask(SIG_BLOCK, &set, nullptr) == 0) {
        std::cout << "SIGUSR2 blocked in main thread" << std::endl;
    }
    
    /// Send SIGUSR2 - it should be queued but not delivered to this thread
    std::cout << "Sending SIGUSR2 (should be blocked in main thread)" << std::endl;
    kill(getpid(), SIGUSR2);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    /// Unblock SIGUSR2
    if (pthread_sigmask(SIG_UNBLOCK, &set, nullptr) == 0) {
        std::cout << "SIGUSR2 unblocked - signal should now be delivered" << std::endl;
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

/// Function to create child process and demonstrate SIGCHLD
void demonstrate_sigchld() {
    std::cout << "\n=== Demonstrating SIGCHLD ===" << std::endl;
    
    pid_t child_pid = fork();
    
    if (child_pid == 0) {
        /// Child process
        std::cout << "Child process (PID: " << getpid() << ") running..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "Child process exiting" << std::endl;
        exit(42);
    } else if (child_pid > 0) {
        /// Parent process
        std::cout << "Parent created child with PID: " << child_pid << std::endl;
        
        /// Wait for child to complete
        int status;
        waitpid(child_pid, &status, 0);
        
        if (WIFEXITED(status)) {
            std::cout << "Child exited with status: " << WEXITSTATUS(status) << std::endl;
        }
    } else {
        perror("fork");
    }
}

int main() {
    std::cout << "=== Linux Signals and Process/Thread Model Test ===" << std::endl;
    std::cout << "Main process PID: " << getpid() << std::endl;
    std::cout << "Main thread TID: " << syscall(SYS_gettid) << std::endl;
    
    /// Setup signal handlers
    setup_signal_handlers();
    
    std::cout << "\n=== Creating Worker Threads ===" << std::endl;
    
    /// Create worker threads
    std::vector<std::thread> threads;
    const int num_threads = 3;
    
    for (int i = 1; i <= num_threads; i++) {
        threads.emplace_back(worker_thread, i);
    }
    
    /// Let threads start up
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    /// Demonstrate signal masking
    demonstrate_signal_masking();
    
    /// Send some signals
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "\nSending SIGUSR2 to process..." << std::endl;
    kill(getpid(), SIGUSR2);
    
    /// Demonstrate SIGCHLD
    demonstrate_sigchld();
    
    /// Set up an alarm
    std::cout << "\nSetting alarm for 3 seconds..." << std::endl;
    alarm(3);
    
    /// Main loop
    std::cout << "\n=== Main Loop Running ===" << std::endl;
    std::cout << "Send signals to test (try: kill -USR1 " << getpid() 
              << " or kill -USR2 " << getpid() << ")" << std::endl;
    std::cout << "Press Ctrl+C to exit gracefully" << std::endl;
    
    int main_counter = 0;
    while (keep_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        main_counter++;
        
        if (main_counter % 10 == 0) {
            std::cout << "[Main] Running for " << main_counter 
                      << " seconds, received " << signal_count.load() 
                      << " signals total" << std::endl;
        }
        
        /// Send a signal every 15 seconds for testing
        if (main_counter % 15 == 0) {
            std::cout << "[Main] Sending test SIGUSR2" << std::endl;
            kill(getpid(), SIGUSR2);
        }
    }
    
    std::cout << "\nShutting down gracefully..." << std::endl;
    
    /// Wait for all threads to complete
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    
    std::cout << "Total signals received: " << signal_count.load() << std::endl;
    std::cout << "Program completed successfully" << std::endl;
    
    return 0;
}
