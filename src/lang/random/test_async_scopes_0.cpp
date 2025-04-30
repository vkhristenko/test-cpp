#include <iostream>
#include <deque>
#include <unordered_set>
#include <functional>
#include <memory>

struct IAsyncScope {
    void Add(IAsyncScope* obj) {
        children.insert(obj);
    }

    void Remove(IAsyncScope* obj) {
        children.erase(obj);

        if (children.empty() and detached_) {
            handleReadyForDestruction();
        }
    }

    virtual void handleReadyForDestruction() = 0;
    virtual void handleStartDestroyParent() = 0;

    void StartDestroy() {
        detached_ = true;

        for (auto child : children) {
            child->handleStartDestroyParent();
        }

        if (children.empty()) {
            handleReadyForDestruction();
        }
    }

    std::unordered_set<IAsyncScope*> children;
    bool detached_ = false;

protected:
    virtual ~IAsyncScope() {}
};

struct InitiateDestructionDeleter {
    void operator()(IAsyncScope* scope) {
        scope->StartDestroy();
    }
};

struct ExecutionContext : public IAsyncScope {
    ~ExecutionContext() {
        detached_ = true;

        while (! children.empty() or !ops.empty()) {
            Poll();
        }
    }

    void handleStartDestroyParent() override {}

    std::deque<std::function<void()>> ops;

    template<typename T>
    void Enqueue(T&& op) {
        ops.push_back(std::forward<T>(op));
    }
    
    size_t Poll() {
        auto const num_ops = ops.size();

        for (size_t i=0; i<num_ops; i++) {
            auto op = std::move(ops.front());
            ops.pop_front();
            op();
        }

        return num_ops;
    }

protected:
    using IAsyncScope::StartDestroy;
    void handleReadyForDestruction() override {}
};

struct TestAsyncScope : public IAsyncScope {
    TestAsyncScope(ExecutionContext& ctx, IAsyncScope& scope, size_t depth, size_t i) 
        : ctx_{ctx}
        , parent_scope_{scope}
        , depth_{depth}
        , i_{i}
    {
        for (size_t i=0; i<depth_; i++) {
            std::print("  ");
        }
        std::println("constructing depth={} i={}", depth_, i_);
        scope.Add(this);
    }

    ExecutionContext& ctx_;
    IAsyncScope& parent_scope_;
    size_t depth_ = 0;
    size_t i_ = 0;
    size_t counter = 0;

protected:
    void handleReadyForDestruction() override {

        ctx_.Enqueue([this] {
            counter++;

            if (counter == depth_*5+i_) {
                TestAsyncScope::~TestAsyncScope();
            }
        });

    }

    void handleStartDestroyParent() override {
         
    }

    ~TestAsyncScope() {
        for (size_t i=0; i<depth_; i++) {
            std::print("  ");
        }
        std::println("destructing depth={} i={}", depth_, i_);
        parent_scope_.Remove(this);
    }
};

void Test0() {
    ExecutionContext ctx;

    std::vector<std::unique_ptr<TestAsyncScope, InitiateDestructionDeleter>> scopes;
    auto generate = [&](size_t idepth, size_t iwidth, auto gen, auto& current_scope) {
        if (idepth == 3) {
            return ;
        }

        for (size_t i=0; i<5; i++) {
            std::unique_ptr<TestAsyncScope, InitiateDestructionDeleter> scope(new TestAsyncScope{ctx, current_scope, idepth, iwidth*5+i});
            
            gen(idepth+1, i, gen, *scope);
            scopes.push_back(std::move(scope));
        }
    };
    generate(0, 0, generate, ctx);
    
    std::println("calling context destructor");
}

int main() {
    Test0();

    return 0;
}
