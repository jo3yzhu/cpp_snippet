#include <event_loop/tasks.h>
#include <unistd.h>

const static char kHelloWorld[] = "hello world!";

void ConnectTask::Init() {
    // Create socket
    sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd_ < 0) {
        throw std::runtime_error("socket() failed");
    }

    // Set non-blocking
    int flags = fcntl(sock_fd_, F_GETFL, 0);
    if (flags < 0) {
        throw std::runtime_error("fcntl(F_GETFL) failed");
    }

    // Set TCP_NODELAY
    if (fcntl(sock_fd_, F_SETFL, flags | O_NONBLOCK) < 0) {
        throw std::runtime_error("fcntl(F_SETFL) failed");
    }

    // Prepare address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    if (inet_pton(AF_INET, ip_, &addr.sin_addr) < 0) {
        throw std::runtime_error("inet_pton() failed");
    }

    // Start non-blocking connect
    if (connect(sock_fd_, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        if (errno != EINPROGRESS) {
            throw std::runtime_error("connect() failed");
        }
    }

    // Register to epoll
    struct epoll_event event;
    event.events = EPOLLOUT;
    event.data.fd = sock_fd_;
    if (epoll_ctl(shared_epoll_fd_, EPOLL_CTL_ADD, sock_fd_, &event) == -1) {
        throw std::runtime_error("epoll_ctl() failed");
    }

    status_ = Status::kPolling;
}

Task::Status ConnectTask::Poll() noexcept {
    // Busy polling for connection status
    struct epoll_event event;
    if (epoll_wait(shared_epoll_fd_, &event, 1, 0) < 0) {
        status_ = Status::kError;
        // TODO: log error of errno
        return status_;
    }

    // Event not ready
    if (!(event.events & EPOLLOUT)) {
        status_ = Status::kPolling;
        return status_;
    }

    // Event triggered, check connection status
    int error;
    socklen_t len = sizeof(error);
    if (getsockopt(sock_fd_, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
        status_ = Status::kError;
        return status_;
    }
    if (error != 0) {
        status_ = Status::kError;
        return status_;
    }

    // Connection established
    event.events = EPOLLIN | EPOLLOUT;
    if (epoll_ctl(shared_epoll_fd_, EPOLL_CTL_MOD, sock_fd_, &event) < 0) {
        status_ = Status::kError;
        return status_;
    }

    // Get ready for next task
    status_ = Status::kReadyNext;
    return status_;
}

std::unique_ptr<Task> ConnectTask::Advance() noexcept {
    return std::make_unique<WriteTask>(sock_fd_, kHelloWorld, shared_epoll_fd_);
}

void WriteTask::Init() {
    // Register to epoll
    event_.events = EPOLLOUT;
    event_.data.fd = sock_fd_;
    if (epoll_ctl(shared_epoll_fd_, EPOLL_CTL_MOD, sock_fd_, &event_) < 0) {
        throw std::runtime_error("epoll_ctl() failed");
    }

    status_ = Status::kPolling;
}

Task::Status WriteTask::Poll() noexcept {
    // Poll for write status
    if (epoll_wait(shared_epoll_fd_, &event_, 1, 0) < 0) {
        status_ = Status::kError;
        return status_;
    }

    // Event not ready
    if (!(event_.events & EPOLLOUT)) {
        status_ = Status::kPolling;
        return status_;
    }

    // Event ready, write as much as possible
    while (remaining_size_ > 0) {
        ssize_t written = write(sock_fd_, remaining_data_, remaining_size_);
        if (written < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Socket buffer full, need to wait
                status_ = Status::kPolling;
                return status_;
            }
            // Interrupted by signal, retry
            if (errno == EINTR) {
                continue;
            }
            // Write error
            status_ = Status::kError;
            return status_;
        }
        remaining_data_ += written;
        remaining_size_ -= written;
    }

    // Write completed
    printf("Send completed: %s\n", echo_data_.data());
    status_ = Status::kReadyNext;
    return status_;
}

std::unique_ptr<Task> WriteTask::Advance() noexcept {
    return std::make_unique<ReadTask>(sock_fd_, sizeof(kHelloWorld), shared_epoll_fd_);
}

void ReadTask::Init() {
    // Register to epoll
    event_.events = EPOLLIN;
    event_.data.fd = sock_fd_;
    if (epoll_ctl(shared_epoll_fd_, EPOLL_CTL_MOD, sock_fd_, &event_) < 0) {
        throw std::runtime_error("epoll_ctl() failed");
    }

    status_ = Status::kPolling;
}

Task::Status ReadTask::Poll() noexcept {
    // Poll for read status
    if (epoll_wait(shared_epoll_fd_, &event_, 1, 0) < 0) {
        status_ = Status::kError;
        return status_;
    }

    // Event not ready
    if (!(event_.events & EPOLLIN)) {
        status_ = Status::kPolling;
        return status_;
    }

    // Event ready, read as much as possible
    while (total_read_ < buffer_size_) {
        ssize_t read_bytes = read(sock_fd_, buffer_ + total_read_, buffer_size_ - total_read_);
        if (read_bytes < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No more data to read, need to wait
                status_ = Status::kPolling;
                return status_;
            }
            // Interrupted by signal, retry
            if (errno == EINTR) {
                continue;
            }
            // Read error
            status_ = Status::kError;
            return status_;
        }

        total_read_ += read_bytes;
    }

    // Read completed
    printf("Receive completed: %s\n", buffer_);
    status_ = Status::kReadyNext;
    return status_;
}

std::unique_ptr<Task> ReadTask::Advance() noexcept {
    return std::make_unique<WriteTask>(sock_fd_, "hello world!", shared_epoll_fd_);
}