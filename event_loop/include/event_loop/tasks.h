#pragma once

#include <arpa/inet.h>
#include <event_loop/task.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string_view>

class ConnectTask : public Task {
public:
    ConnectTask(const char* ip, uint16_t port, int shared_epoll_fd) noexcept
        : ip_(ip), port_(port), sock_fd_(-1), shared_epoll_fd_(shared_epoll_fd) {}

    void Init() override;
    Task::Status Poll() noexcept override;
    std::unique_ptr<Task> Advance() noexcept override;

private:
    const char* ip_;
    uint16_t port_;
    int sock_fd_;
    int shared_epoll_fd_;
};

class WriteTask : public Task {
public:
    WriteTask(int sock_fd, std::string_view echo_data, int shared_epoll_fd) noexcept
        : sock_fd_(sock_fd),
          echo_data_(echo_data),
          shared_epoll_fd_(shared_epoll_fd),
          remaining_size_(echo_data_.size()),
          remaining_data_(echo_data_.data()) {}

    void Init() override;
    Task::Status Poll() noexcept override;
    std::unique_ptr<Task> Advance() noexcept override;

private:
    std::string_view echo_data_;
    size_t remaining_size_;
    const char* remaining_data_;
    int sock_fd_;
    int shared_epoll_fd_;
    epoll_event event_;
};

class ReadTask : public Task {
public:
    ReadTask(int sock_fd, size_t buffer_size, int shared_epoll_fd) noexcept
        : sock_fd_(sock_fd),
          buffer_size_(buffer_size),
          shared_epoll_fd_(shared_epoll_fd) {}

    void Init() override;
    Task::Status Poll() noexcept override;
    std::unique_ptr<Task> Advance() noexcept override;

private:
    size_t buffer_size_;
    size_t total_read_;
    char buffer_[1024];
    int sock_fd_;
    int shared_epoll_fd_;
    epoll_event event_;
};