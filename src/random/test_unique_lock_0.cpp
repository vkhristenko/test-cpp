#include <condition_variable>
#include <iostream>
#include <thread>

std::mutex g_mu;
bool g_done_1{false}, g_done_2{false};
std::condition_variable g_cv;

void WaitingThreadRunner() {
    {
        std::unique_lock<std::mutex> lck{g_mu};
        while (!g_done_1) {
            g_cv.wait(lck);
        }
        std::cout << __LINE__ << std::endl;
    }

    {
        std::lock_guard<std::mutex> lck{g_mu};
        g_done_2 = true;
        std::cout << __LINE__ << std::endl;
    }
    g_cv.notify_one();
}

int main() {
    std::thread t{WaitingThreadRunner};

    {
        std::lock_guard<std::mutex> lck{g_mu};
        g_done_1 = true;
        std::cout << __LINE__ << std::endl;
    }
    g_cv.notify_one();

    {
        std::unique_lock<std::mutex> lck{g_mu};
        while (!g_done_2) {
            g_cv.wait(lck);
        }
        std::cout << __LINE__ << std::endl;
    }

    t.join();

    return 0;
}
