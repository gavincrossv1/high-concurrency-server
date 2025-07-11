#pragma once
#include <unordered_map>
#include <functional>
#include <chrono>
#include <mutex>

class ConnectionTimeoutManager
{
public:
    void addConnection(int fd, std::function<void()> timeoutCallback, int timeoutSeconds);
    void removeConnection(int fd);
    void checkTimeouts();

private:
    struct ConnectionInfo
    {
        std::chrono::steady_clock::time_point expireTime;
        std::function<void()> timeoutCallback;
    };
    std::unordered_map<int, ConnectionInfo> m_connections;
    std::mutex m_mutex;
};
