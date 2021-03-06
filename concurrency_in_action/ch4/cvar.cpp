#include <thread>
#include <queue>
#include <mutex>

struct data_chunk {};

void test0();

int main() {
}

std::mutex mut;
std::queue<data_chunk> data_queue;
std::condition_variable data_cond;

bool more_data_to_prepare() {
    static int counter = 0;
    counter++;
    return counter < 5;
}

data_chunk prepare_data() { return {}; }

void data_preparation_thread() {
    while (more_data_to_prepare()) {
        data_chunk const data = prepare_data();
        std::lock_guard<std::mutex> lk{mut};
        data_queue.push(data);
        data_cond.notify_one();
    }
}

bool is_last_chunk(data_chunk const&) {
    static int counter = 0;
    counter++;
    return counter < 5;
}

void process(data_chunk const&) {}

void data_processing_thread() {
    while (true) {
        std::unique_lock<std::mutex> lk{mut};
        data_cond.wait(lk, []{ return !data_queue.empty(); });
        auto data = data_queue.front();
        data_queue.pop();
        lk.unlock();
        process(data);
        if (is_last_chunk(data)) break;
    }
}

void test0() {
    std::thread t1{data_preparation_thread}, t2{data_processing_thread};
    t1.join(); t2.join();
}
