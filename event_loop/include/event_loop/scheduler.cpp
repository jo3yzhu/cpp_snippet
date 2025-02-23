#include "scheduler.h"

void Scheduler::Schedule() {
    if (queue_.empty()) {
        return;
    }

    auto task = queue_.front();
    task.Execute();
    auto next = task.Poll();
    if (next.has_value()) {
        queue_.push_back(next.value());
    }
    queue_.pop_front();
}