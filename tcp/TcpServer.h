#pragma once
#include "EventLoop.h"
#include "ThreadPool.h"

class TcpServer
{
public:
    TcpServer(unsigned short port, int threadNum);
    // 初始化监听
    void setListen();
    // 启动服务器
    void run();
    static int acceptConnection(void *arg);

    // Getter/Setter
    unsigned short getPort() const { return m_port; }
    void setPort(unsigned short port) { m_port = port; }
    int getListenFd() const { return m_lfd; }
    void setListenFd(int fd) { m_lfd = fd; }
    int getThreadNum() const { return m_threadNum; }
    void setThreadNum(int num) { m_threadNum = num; }
    EventLoop *getMainLoop() const { return m_mainLoop; }
    ThreadPool *getThreadPool() const { return m_threadPool; }

private:
    int m_threadNum;
    EventLoop *m_mainLoop;
    ThreadPool *m_threadPool;
    int m_lfd;
    unsigned short m_port;
};
