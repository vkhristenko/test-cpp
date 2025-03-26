#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>

void Test0() {

    std::mutex mu;
    std::condition_variable cv;
    std::string data;
    bool ready = false;
    bool processed = false;
    
    auto one = [&] {
        std::unique_lock lk{mu};
        cv.wait(lk, [&]{return ready;});

        std::cout << "worker thread is processing" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        data += "after processing";

        processed = true;
        std::cout << "done processing" << std::endl;

        lk.unlock();
        cv.notify_one();
    };

    auto two = [&] {
        {
            std::lock_guard lk{mu};
            data = "some data ";
            ready = true;
        }
        cv.notify_one();

        {
            std::unique_lock lk{mu};
            cv.wait(lk, [&]{ return processed; });
            std::cout << data << std::endl;
        }
    };

    std::thread t1{one};
    std::thread t2{two};

    t1.join(); t2.join();
}

int main() {
    Test0();

    return 0;
}
