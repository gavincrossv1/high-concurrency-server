#pragma once
#include "Channel.h"
#include "EventLoop.h"
#include "Dispatcher.h"
#include <string>
#include <poll.h>
using namespace std;

class PollDispatcher : public Dispatcher
{
public:
    PollDispatcher(EventLoop *evloop);
    ~PollDispatcher();
    // 添加
    int add() override;
    // 删除
    int remove() override;
    // 修改
    int modify() override;
    // 事件监测
    int dispatch(int timeout = 2) override; // 单位: s

    // 获取 pollfd 数组
    inline struct pollfd *getPollFds() const { return m_fds; }
    // 获取最大节点数
    inline int getMaxNode() const { return m_maxNode; }
    // 获取最大 fd
    inline int getMaxFd() const { return m_maxfd; }
    // 设置最大 fd
    inline void setMaxFd(int maxfd) { m_maxfd = maxfd; }

private:
    int m_maxfd;
    struct pollfd *m_fds;
    const int m_maxNode = 1024;
};