#include <event_loop/scheduler.h>

void Scheduler::Submit(std::unique_ptr<Task> &&task) {
    task->Init();
    queue_.emplace_back(std::move(task));
}

void Scheduler::Schedule() {
    if (queue_.empty()) {
        return;
    }

    auto task = std::move(queue_.front());
    queue_.pop_front();
    auto status = task->Poll();
    switch (status) {
        case Task::Status::kReadyNext:
            queue_.pop_front();
            Submit(task->Advance());
            break;
        case Task::Status::kPolling:
            queue_.push_back(std::move(task));
            break;
        case Task::Status::kError:
            // fatal log here
            break;
        default:
            // fatal log here
            break;
    }
}