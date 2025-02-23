#include <optional>
#include <functional>

class Task {
public:
    virtual void Execute() noexcept;
    virtual std::optional<Task> Poll() noexcept;
private:
    std::function<void()> f_;
};