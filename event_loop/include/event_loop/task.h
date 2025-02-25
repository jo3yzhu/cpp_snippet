#pragma once

#include <memory>

class alignas(64) Task {
public:
    enum class Status : uint8_t {
        kInit,
        kPolling,
        kReadyNext,
        kReadyDone,
        kError
    };

public:
    Task() noexcept : status_(Status::kInit) {}
    Task(Task&& other) noexcept = default;
    Task& operator=(Task&& other) noexcept = default;
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    virtual ~Task() noexcept = default;
    virtual void Init() = 0;
    [[nodiscard]] virtual Status Poll() noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<Task> Advance() noexcept = 0;
    [[nodiscard]] Status GetStatus() const noexcept {
        return status_;
    }

protected:
    Task::Status status_;
};