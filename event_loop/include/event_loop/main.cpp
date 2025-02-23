#include <event_loop/scheduler.h>
#include <event_loop/poller.h>

int main(int argc, char** argv) {
    bool shutdown_flag = false;
    Scheduler scheduler;
    
    scheduler.Submit()
    
    while (!shutdown_flag) {
        scheduler.Schedule();
    }
    return 0;
}