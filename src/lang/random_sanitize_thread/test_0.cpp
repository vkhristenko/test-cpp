#include <pthread.h>

int Global;

void* Thread1(void* x) {
    Global = 1;
    return nullptr;
}

int main() {
    pthread_t t1;
    pthread_create(&t1, nullptr, Thread1, nullptr);
    Global = 2;
    pthread_join(t1, nullptr);
    return Global;
}
