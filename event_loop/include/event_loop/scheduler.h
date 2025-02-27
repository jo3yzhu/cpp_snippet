#pragma once

#include <event_loop/task.h>
#include <memory>
#include <list>

class Scheduler {
public:
    void Submit(std::unique_ptr<Task> &&task); // push_back
    void Urgent(std::unique_ptr<Task> &&task); // push_front
    void Schedule();
private:
    std::list<std::unique_ptr<Task>> queue_;
};