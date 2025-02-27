#include <iostream>
#include <coroutine>

struct resumable {
    struct promise_type {
        resumable get_return_object() {
            return resumable{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        auto initial_suspend() {
            return std::suspend_never{};
        }
        auto final_suspend() noexcept {
            return std::suspend_never{};
        }
        void return_void() {}
        void unhandled_exception() {}
    };
    std::coroutine_handle<promise_type> handle_ = nullptr;
    resumable() = default;
    resumable(resumable const&) = delete;
    resumable& operator=(resumable const&) = delete;
    resumable(resumable &&rhs) : handle_(rhs.handle_) {
        rhs.handle_ = nullptr;
    }
    resumable& operator=(resumable &&rhs) {
        if (this != &rhs) {
            handle_ = rhs.handle_;
            rhs.handle_ = nullptr;
        }
        return *this;
    }
    explicit resumable(std::coroutine_handle<promise_type> handle) : handle_(handle) {}
    ~resumable() {
        if (handle_) {
            handle_.destroy();
        }
    }
    void resume() {
        handle_.resume();
    }
};

resumable counter() {
    std::cout << "Counter: called" << std::endl;
    for (int i = 0; i < 5; ++i) {
        co_await std::suspend_always{};
        std::cout << "Counter: resumed #" << i << std::endl;
    }
}

int main(int argc, char *argv[]) {
    resumable res = counter();
    for (int i = 0; i < 5; ++i) {
        res.resume();
    }
    return 0;
}