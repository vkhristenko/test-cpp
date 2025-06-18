#include <iostream>
#include <queue>
#include <string>
#include <vector>

template <typename T>
class Pool {
public:
    Pool() {}
    Pool(Pool const&);
    Pool& operator=(Pool const&);
    ~Pool() {
        while (!data_.empty()) {
            auto front = data_.front();
            data_.pop();
            delete front;
        }
    }

    struct ReturnToPoolDeleter {
        Pool* pool;

        void operator()(T* ptr) { pool->data_.push(ptr); }
    };
    using Deleter = ReturnToPoolDeleter;

    std::unique_ptr<T, ReturnToPoolDeleter> AllocateAndConstruct() noexcept {
        if (data_.empty()) {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
            /// TODO
            /// raii stuff
            data_.push(new T{});
        }

        auto front = data_.front();
        data_.pop();

        return {front, ReturnToPoolDeleter{this}};
    }

private:
    std::queue<T*> data_;
};

template <typename T>
class PoolForShared {
public:
    PoolForShared() {}

    struct ReturnToPoolDeleter {
        PoolForShared* pool;

        void operator()(T* ptr) { pool->data_.push_back(ptr); }
    };
    using Deleter = ReturnToPoolDeleter;

    std::shared_ptr<T> AllocateAndConstruct() noexcept {
        if (data_.empty()) {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
            /// TODO
            /// raii stuff
            data_.push_back(new T{});
        }

        auto front = data_.front();
        data_.pop_front();
        return {front, ReturnToPoolDeleter{this}};
    }

private:
    std::deque<T*> data_;
};

void TestSharedPool() {
    PoolForShared<std::string> pool;

    {
        std::vector<std::shared_ptr<std::string>> vs;
        for (std::size_t i = 0; i < 10; i++) {
            vs.push_back(pool.AllocateAndConstruct());
        }

        for (auto& v : vs) {
            *v = "some string";
        }

        for (auto const& v : vs) {
            std::cout << *v << std::endl;
        }
    }

    std::cout << "000" << std::endl;

    {
        std::vector<std::shared_ptr<std::string>> vs;
        for (std::size_t i = 0; i < 10; i++) {
            vs.push_back(pool.AllocateAndConstruct());
        }

        for (auto& v : vs) {
            *v = "some string";
        }

        for (auto const& v : vs) {
            std::cout << *v << std::endl;
        }
    }
}

void Test0() {
    Pool<std::string> pool;

    {
        std::vector<std::unique_ptr<std::string, Pool<std::string>::Deleter>>
            vs;
        for (std::size_t i = 0; i < 10; i++) {
            vs.push_back(pool.AllocateAndConstruct());
        }

        for (auto& v : vs) {
            *v = "some string";
        }

        for (auto const& v : vs) {
            std::cout << *v << std::endl;
        }
    }

    std::cout << "000" << std::endl;

    {
        std::vector<std::unique_ptr<std::string, Pool<std::string>::Deleter>>
            vs;
        for (std::size_t i = 0; i < 10; i++) {
            vs.push_back(pool.AllocateAndConstruct());
        }

        for (auto& v : vs) {
            *v = "some string";
        }

        for (auto const& v : vs) {
            std::cout << *v << std::endl;
        }
    }
}

int main() {
    Test0();
    TestSharedPool();

    return 0;
}
