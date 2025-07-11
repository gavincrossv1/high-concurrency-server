#pragma once
#include "EventLoop.h"
#include "Buffer.h"
#include "Channel.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

// #define MSG_SEND_AUTO

class TcpConnection
{
public:
    TcpConnection(int fd, EventLoop *evloop);
    ~TcpConnection();

    static int processRead(void *arg);
    static int processWrite(void *arg);
    static int destroy(void *arg);

    // Getter/Setter
    const string &getName() const { return m_name; }
    void setName(const string &name) { m_name = name; }
    EventLoop *getEventLoop() const { return m_evLoop; }
    Channel *getChannel() const { return m_channel; }
    Buffer *getReadBuf() const { return m_readBuf; }
    Buffer *getWriteBuf() const { return m_writeBuf; }
    HttpRequest *getRequest() const { return m_request; }
    HttpResponse *getResponse() const { return m_response; }

private:
    string m_name;
    EventLoop *m_evLoop;
    Channel *m_channel;
    Buffer *m_readBuf;
    Buffer *m_writeBuf;
    // http 协议
    HttpRequest *m_request;
    HttpResponse *m_response;
};