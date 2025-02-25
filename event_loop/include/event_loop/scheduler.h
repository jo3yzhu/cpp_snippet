#pragma once

#include <event_loop/task.h>
#include <memory>
#include <list>

class Scheduler {
public:
    void Submit(std::unique_ptr<Task> &&task);
    void Schedule();
private:
    std::list<std::unique_ptr<Task>> queue_;
};