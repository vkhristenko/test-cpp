#include <condition_variable>
#include <iostream>
#include <thread>

std::mutex g_mu;
bool g_done_1 {false}, g_done_2{false};
std::condition_variable g_cv;

void WaitingThreadRunner() {
    {
        std::unique_lock<std::mutex> lck{g_mu};
        std::this_thread::sleep_for(std::chrono::seconds(1));
        g_cv.wait(lck);
    }

    std::cout << __LINE__ << std::endl;
}

int main() {
    std::thread t{WaitingThreadRunner};

    g_cv.notify_one();

    std::cout << __LINE__ << std::endl;

    t.join();

    return 0;
}
