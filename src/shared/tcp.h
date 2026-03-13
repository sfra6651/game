#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <functional>
#include <thread>
#include <atomic>
#include <vector>

#define DEBUG_PORT 9999

// Tools side: listens for a game connection
struct TcpServer {
    int listenFd_ = -1;
    int clientFd_ = -1;
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
            close(listenFd_);
            listenFd_ = -1;
            return false;
        }

        if (listen(listenFd_, 1) < 0) {
            close(listenFd_);
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
                        if (clientFd_ >= 0) close(clientFd_);
                        clientFd_ = fd;
                        connected_ = true;
                    }
                }
            }
        });

        return true;
    }

    // Read exactly n bytes from the client. Returns false on disconnect.
    bool recv(void *buf, int n) {
        if (clientFd_ < 0) return false;
        int received = 0;
        while (received < n) {
            int r = ::recv(clientFd_, (char *)buf + received, n - received, 0);
            if (r <= 0) {
                connected_ = false;
                close(clientFd_);
                clientFd_ = -1;
                return false;
            }
            received += r;
        }
        return true;
    }

    bool isConnected() const { return connected_; }

    void stop() {
        running_ = false;
        if (acceptThread_.joinable()) acceptThread_.join();
        if (clientFd_ >= 0) close(clientFd_);
        if (listenFd_ >= 0) close(listenFd_);
        clientFd_ = -1;
        listenFd_ = -1;
    }

    ~TcpServer() { stop(); }
};

// Game side: connects to the tools server
struct TcpClient {
    int fd_ = -1;

    bool connect() {
        fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ < 0) return false;

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        addr.sin_port = htons(DEBUG_PORT);

        if (::connect(fd_, (sockaddr *)&addr, sizeof(addr)) < 0) {
            close(fd_);
            fd_ = -1;
            return false;
        }
        return true;
    }

    // Send exactly n bytes. Returns false on disconnect.
    bool send(const void *buf, int n) {
        if (fd_ < 0) return false;
        int sent = 0;
        while (sent < n) {
            int s = ::send(fd_, (const char *)buf + sent, n - sent, MSG_NOSIGNAL);
            if (s <= 0) {
                close(fd_);
                fd_ = -1;
                return false;
            }
            sent += s;
        }
        return true;
    }

    bool isConnected() const { return fd_ >= 0; }

    void disconnect() {
        if (fd_ >= 0) close(fd_);
        fd_ = -1;
    }

    ~TcpClient() { disconnect(); }
};
