#pragma once
#include "Channel.h"
#include "EventLoop.h"
#include "Dispatcher.h"
#include <string>
#include <sys/epoll.h>
using namespace std;

class EpollDispatcher : public Dispatcher
{
public:
    EpollDispatcher(EventLoop *evloop);
    ~EpollDispatcher();
    // 添加
    int add() override;
    // 删除
    int remove() override;
    // 修改
    int modify() override;
    // 事件监测
    int dispatch(int timeout = 2) override; // 单位: s

    // 获取 epoll fd
    inline int getEpollFd() const { return m_epfd; }
    // 获取 epoll_event 数组
    inline struct epoll_event *getEvents() const { return m_events; }
    // 获取最大节点数
    inline int getMaxNode() const { return m_maxNode; }

private:
    int epollCtl(int op);

private:
    int m_epfd;
    struct epoll_event *m_events;
    const int m_maxNode = 1024;
};