#pragma once
#include "EventLoop.h"
#include "Buffer.h"
#include "Channel.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <chrono>

// #define MSG_SEND_AUTO

class TcpConnection
{
public:
    TcpConnection(int fd, EventLoop *evloop, bool keepAlive, int keepAliveTimeout);
    ~TcpConnection();

    static int processRead(void *arg);
    static int processWrite(void *arg);
    static int processTimeout(void *arg);
    static int destroy(void *arg);

private:
    string m_name;
    EventLoop *m_evLoop;
    Channel *m_channel;
    Buffer *m_readBuf;
    Buffer *m_writeBuf;
    // http 协议
    HttpRequest *m_request;
    HttpResponse *m_response;
    bool m_keepAlive;
    bool m_closeAfterWrite;
    int m_keepAliveTimeout;
    chrono::steady_clock::time_point m_lastActiveTime;
};