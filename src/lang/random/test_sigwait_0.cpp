#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>

void RunThread() {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    std::cout << "Thread waiting for SIGINT..." << std::endl;
    int sig;
    int result = sigwait(&set, &sig);
    if (result == 0) {
        std::cout << "Thread received signal: " << sig << std::endl;
    } else {
        std::cout << "sigwait failed with error: " << result << std::endl;
    }
}

int main() {
    /// CORRECTLY block SIGINT in all threads
    /// This is crucial - block the signal BEFORE creating threads
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);  /// Add SIGINT to the set
    
    /// Block SIGINT in main thread (child threads inherit this)
    if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
        perror("pthread_sigmask");
        return 1;
    }
    
    std::cout << "SIGINT blocked in all threads" << std::endl;

    pthread_t thread;
    
    // Create a thread that will wait for signals
    if (pthread_create(&thread, NULL, (void* (*)(void*))RunThread, NULL) != 0) {
        perror("pthread_create");
        return 1;
    }

    /// Give the thread time to start and call sigwait()
    std::cout << "Waiting for thread to start..." << std::endl;
    sleep(2); 
    
    std::cout << "Sending SIGINT to process (PID: " << getpid() << ")..." << std::endl;
    kill(getpid(), SIGINT);  

    // Wait for the thread to finish
    pthread_join(thread, NULL);

    printf("Thread finished execution.\n");
    return 0;
}
