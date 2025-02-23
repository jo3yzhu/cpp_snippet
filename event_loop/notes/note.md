# Overview
## Key Design
1. EventLoop per Thread
2. EventLoop has one or more Poller
3. EventLoop communicates with each other
4. EventLoop -> Poller -> fd(s)
5. Accurate timers, instance numeber < 100

## API
### tradeoff
- coroutine-like (context_t)
    - context_t + heap allocation
    - 
- std::function<>
    - callback
    - rare cross-thread scenario


### Strcucture
EventLoop(busy io_uring, bind to an I/O thread)
    |-> NetworkPoller
    |-> TimerPoller
    |-> TscnsPoller
    |-> ...

Poller communicates with each other using RingBuffer(lossy), scenario?

EventLoop communicates with each other using lock-free queue(strict non-lossy), scenario?



# Future
1. bypass?
2. urgent/defer/post, submit first

# Note
TakeQueue lock-free list, node implementation like snmalloc
No cross-thread destruction: free list node at where it's created


# Note2
1. cross thread overhead (submit_task, function object, gc)
2. 99% network task complete in-thread  (function object allocator)
3. priority task queue (cancel fast)
4. Poller abstraction