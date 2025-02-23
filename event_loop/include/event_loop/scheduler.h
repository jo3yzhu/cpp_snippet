#include <event_loop/task.h>
#include <functional>
#include <vector>
#include <list>


class Scheduler {
public:
    void Schedule();
    void Submit();
private:
    std::list<Task> queue_;
};