#include <sys/epoll.h>
#include <event_loop/scheduler.h>
#include <event_loop/tasks.h>

void Run(int shared_epoll_fd) {
    bool shutdown_flag = false;
    Scheduler scheduler;
    scheduler.Submit(std::make_unique<ConnectTask>("127.0.0.1", 12345, shared_epoll_fd));
    
    while (!shutdown_flag) {
        scheduler.Schedule();
    }
}

int main(int argc, char* argv[]) {
    int shared_epoll_fd = epoll_create1(0);   
    Run(shared_epoll_fd);
    return 0;
}