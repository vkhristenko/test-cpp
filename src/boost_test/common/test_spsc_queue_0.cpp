#include <boost/lockfree/spsc_queue.hpp>
#include <iostream>
#include <thread>
#include <vector>

struct IVisitor;

struct IMsg {
    virtual ~IMsg() = default;

    void in_use(bool value) { in_use_ = value; }

    bool in_use() const { return in_use_; }

    void setExtra(void* e) { extra_ = e; }

    void* getExtra() const { return extra_; }

    virtual void Accept(IVisitor&);

    std::atomic<bool> in_use_{false};
    void* extra_{nullptr};
};

struct Msg1;
struct Msg2;

struct IVisitor {
    virtual void Visit(IMsg&) { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    virtual void Visit(Msg1&) { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    virtual void Visit(Msg2&) { std::cout << __PRETTY_FUNCTION__ << std::endl; }
};

template <typename T>
struct EnableMsgWithVisitor : IMsg {
    void Accept(IVisitor& visitor) override {
        visitor.Visit(static_cast<T&>(*this));
    }
};

struct IMsgDeleter {
    void operator()(IMsg* msg) {
        if (msg) {
            msg->in_use(false);
        }
    }
};

struct Msg1 : EnableMsgWithVisitor<Msg1> {};
struct Msg2 : EnableMsgWithVisitor<Msg2> {};

void IMsg::Accept(IVisitor& visitor) { visitor.Visit(*this); }

template <typename T>
struct DefaultDeleter {
    template <typename U>
    DefaultDeleter(DefaultDeleter<U>&&) {}

    template <typename U>
    DefaultDeleter<T>& operator=(DefaultDeleter<U>&&) {
        return *this;
    }

    void operator()(T* t) { delete t; }
};

template <typename T>
struct Pool {
    std::vector<std::unique_ptr<T>> data_;

    std::unique_ptr<T, IMsgDeleter> AllocateAndConstruct() {
        for (auto& msg : data_) {
            if (!msg->in_use()) {
                msg->in_use(true);
                return std::unique_ptr<T, IMsgDeleter>{msg.get(),
                                                       IMsgDeleter{}};
            }
        }

        auto& msg = data_.emplace_back();
        msg->in_use(true);
        return std::unique_ptr<T, IMsgDeleter>{msg.get(), IMsgDeleter{}};
    }

    Pool() : data_(100) {
        for (auto& msg : data_) {
            msg = std::make_unique<T>();
        }
    }
};

template <typename T, typename Deleter = DefaultDeleter<T>>
struct UniquePtrWrapper {
    UniquePtrWrapper(std::unique_ptr<T, Deleter>&& ptr)
        : ptr_{std::move(ptr)} {}

    UniquePtrWrapper(UniquePtrWrapper const& rhs) {
        ptr_ = std::move(const_cast<UniquePtrWrapper&>(rhs).ptr_);
    }
    UniquePtrWrapper& operator=(UniquePtrWrapper const& rhs) {
        ptr_ = std::move(const_cast<UniquePtrWrapper&>(rhs).ptr_);
        return *this;
    }
    UniquePtrWrapper(UniquePtrWrapper&&) = default;
    UniquePtrWrapper& operator=(UniquePtrWrapper&&) = default;

    std::unique_ptr<T, Deleter> ptr_;
};

void Test1() {
    auto queue =
        boost::lockfree::spsc_queue<UniquePtrWrapper<IMsg, IMsgDeleter>>(
            100000);
    Pool<IMsg> default_pool;
    Pool<Msg1> msg1_pool;
    Pool<Msg2> msg2_pool;
    auto visitor = IVisitor{};

    auto producer = std::thread{[&] {
        size_t i = 0;
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            std::unique_ptr<IMsg, IMsgDeleter> msg;
            if (i % 3 == 0) {
                msg = default_pool.AllocateAndConstruct();
            } else if (i % 3 == 1) {
                msg = msg1_pool.AllocateAndConstruct();
            } else {
                msg = msg2_pool.AllocateAndConstruct();
            }
            msg->setExtra(reinterpret_cast<IVisitor*>(&visitor));
            i++;

            if (!queue.push(std::move(msg))) {
                std::cout << "can't write" << std::endl;
            } else {
                std::cout << "pushed" << std::endl;
            }
        }
    }};

    auto consumer = std::thread{[&] {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            UniquePtrWrapper<IMsg, IMsgDeleter> msg_wrapper =
                std::unique_ptr<IMsg, IMsgDeleter>{nullptr};
            if (!queue.pop(msg_wrapper)) {
                std::cout << "can't read" << std::endl;
            } else {
                auto msg = std::move(msg_wrapper.ptr_);
                auto visitor = reinterpret_cast<IVisitor*>(msg->getExtra());
                msg->Accept(*visitor);
            }
        }
    }};

    producer.join();
    consumer.join();
}

void Test0() {
    auto queue = boost::lockfree::spsc_queue<int>(100000);

    auto producer = std::thread{[&] {
        int i = 0;
        while (true) {
            if (!queue.push(i++)) {
                std::cout << "can't write" << std::endl;
            } else {
                std::cout << "pushed = " << i - 1 << std::endl;
            }
        }
    }};

    auto consumer = std::thread{[&] {
        int i = 0;
        while (true) {
            if (!queue.pop(i)) {
                std::cout << "can't read" << std::endl;
            } else {
                std::cout << "popped = " << std::endl;
            }
        }
    }};

    producer.join();
    consumer.join();
}

int main() {
    /// Test0();
    Test1();

    return 0;
}
