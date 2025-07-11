#include "ConnectionTimeoutManager.h"

void ConnectionTimeoutManager::addConnection(int fd, std::function<void()> timeoutCallback, int timeoutSeconds)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_connections[fd] = {std::chrono::steady_clock::now() + std::chrono::seconds(timeoutSeconds), timeoutCallback};
}

void ConnectionTimeoutManager::removeConnection(int fd)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_connections.erase(fd);
}

void ConnectionTimeoutManager::checkTimeouts()
{
    auto now = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto it = m_connections.begin(); it != m_connections.end();)
    {
        if (it->second.expireTime <= now)
        {
            it->second.timeoutCallback();
            it = m_connections.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
