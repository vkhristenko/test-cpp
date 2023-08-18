struct ExecutionContext {
    uint32_t Poll() {
        auto to_run = std::move(funcs);
        funcs.clear();

        while (!to_run.empty()) {
            auto c = std::move(to_run.pop());
            to_run.pop();
            c();
        }
    }

    template<typename Callable>
    void Enqueue(Callable&& c) {
        funcs_.push(std::forward(c));
    }

    std::queue<std::function<void()>> funcs_;
};

struct EventService {
    EventService(ExecutionContext&);



    ExecutionContext* ctx_;

};
