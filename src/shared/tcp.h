#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <atomic>

#define DEBUG_PORT 9999

// Wraps a connected socket fd with send/recv for both sides
struct TcpConnection {
    int fd_ = -1;

    bool send(const void *buf, int n) {
        if (fd_ < 0) return false;
        int sent = 0;
        while (sent < n) {
            int s = ::send(fd_, (const char *)buf + sent, n - sent, MSG_NOSIGNAL);
            if (s <= 0) {
                ::close(fd_);
                fd_ = -1;
                return false;
            }
            sent += s;
        }
        return true;
    }

    bool recv(void *buf, int n) {
        if (fd_ < 0) return false;
        int received = 0;
        while (received < n) {
            int r = ::recv(fd_, (char *)buf + received, n - received, 0);
            if (r <= 0) {
                ::close(fd_);
                fd_ = -1;
                return false;
            }
            received += r;
        }
        return true;
    }

    // Returns true if there is data waiting to be read (non-blocking check)
    bool hasData() const {
        if (fd_ < 0) return false;
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd_, &fds);
        timeval tv{0, 0}; // immediate, no wait
        return select(fd_ + 1, &fds, nullptr, nullptr, &tv) > 0;
    }

    bool isConnected() const { return fd_ >= 0; }

    void close() {
        if (fd_ >= 0) ::close(fd_);
        fd_ = -1;
    }

    ~TcpConnection() { close(); }
};

// Tools side: listens for a game connection
struct TcpServer {
    int listenFd_ = -1;
    TcpConnection conn_;
    std::thread acceptThread_;
    std::atomic<bool> connected_{false};
    std::atomic<bool> running_{false};

    bool start() {
        listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (listenFd_ < 0) return false;

        int opt = 1;
        setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        addr.sin_port = htons(DEBUG_PORT);

        if (bind(listenFd_, (sockaddr *)&addr, sizeof(addr)) < 0) {
            ::close(listenFd_);
            listenFd_ = -1;
            return false;
        }

        if (listen(listenFd_, 1) < 0) {
            ::close(listenFd_);
            listenFd_ = -1;
            return false;
        }

        running_ = true;
        acceptThread_ = std::thread([this]() {
            while (running_) {
                fd_set fds;
                FD_ZERO(&fds);
                FD_SET(listenFd_, &fds);
                timeval tv{0, 200000}; // 200ms timeout
                int ret = select(listenFd_ + 1, &fds, nullptr, nullptr, &tv);
                if (ret > 0) {
                    int fd = accept(listenFd_, nullptr, nullptr);
                    if (fd >= 0) {
                        conn_.close();
                        conn_.fd_ = fd;
                        connected_ = true;
                    }
                }
            }
        });

        return true;
    }

    TcpConnection& connection() { return conn_; }
    bool isConnected() const { return connected_ && conn_.fd_ >= 0; }

    void stop() {
        running_ = false;
        if (acceptThread_.joinable()) acceptThread_.join();
        conn_.close();
        if (listenFd_ >= 0) ::close(listenFd_);
        listenFd_ = -1;
    }

    ~TcpServer() { stop(); }
};

// Game side: connects to the tools server
struct TcpClient {
    TcpConnection conn_;

    bool connect() {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) return false;

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        addr.sin_port = htons(DEBUG_PORT);

        if (::connect(fd, (sockaddr *)&addr, sizeof(addr)) < 0) {
            ::close(fd);
            return false;
        }
        conn_.fd_ = fd;
        return true;
    }

    TcpConnection& connection() { return conn_; }
    bool isConnected() const { return conn_.fd_ >= 0; }

    void disconnect() { conn_.close(); }

    ~TcpClient() { disconnect(); }
};
