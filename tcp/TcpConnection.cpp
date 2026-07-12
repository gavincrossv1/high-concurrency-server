#include "TcpConnection.h"
#include "HttpRequest.h"
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "Log.h"
#include "BufferPool.h"

int TcpConnection::processRead(void *arg)
{
    TcpConnection *conn = static_cast<TcpConnection *>(arg);
    // 接收数据
    int socket = conn->m_channel->getSocket();
    int count = conn->m_readBuf->socketRead(socket);

    Debug("接收到的http请求数据: %.*s", conn->m_readBuf->readableSize(),
          conn->m_readBuf->data());
    if (count > 0)
    {
        conn->m_lastActiveTime = chrono::steady_clock::now();
        // 接收到了 http 请求, 解析http请求
        // 一个读缓冲中可能包含多个完整请求，逐个处理直到数据不足或需要关闭。
        while (conn->m_readBuf->readableSize() > 0)
        {
            HttpParseResult result = conn->m_request->parseHttpRequest(
                conn->m_readBuf, conn->m_response,
                conn->m_writeBuf, socket, conn->m_keepAlive);
            if (result == HttpParseResult::Incomplete)
            {
                break;
            }
            if (result == HttpParseResult::Close)
            {
                conn->m_evLoop->addTask(conn->m_channel, ElemType::DELETE);
                return 0;
            }
        }
    }
    else
    {
        // 断开连接
        conn->m_evLoop->addTask(conn->m_channel, ElemType::DELETE);
    }
    return 0;
}

int TcpConnection::processTimeout(void *arg)
{
    TcpConnection *conn = static_cast<TcpConnection *>(arg);
    auto idleSeconds = chrono::duration_cast<chrono::seconds>(
        chrono::steady_clock::now() - conn->m_lastActiveTime).count();
    if (idleSeconds >= conn->m_keepAliveTimeout)
    {
        Debug("长连接空闲超时: %s, idle: %ld 秒",
              conn->m_name.data(), static_cast<long>(idleSeconds));
        return 1;
    }
    return 0;
}

// 保留写事件回调，当前直接发送模式不会主动启用 EPOLLOUT。
int TcpConnection::processWrite(void *arg)
{
    Debug("开始发送数据了(基于写事件发送)....");
    TcpConnection *conn = static_cast<TcpConnection *>(arg);
    // 发送数据
    int count = conn->m_writeBuf->sendData(conn->m_channel->getSocket());
    if (count > 0)
    {
        // 判断数据是否被全部发送出去了
        if (conn->m_writeBuf->readableSize() == 0)
        {
            // 1. 不再检测写事件 -- 修改channel中保存的事件
            conn->m_channel->writeEventEnable(false);
            // 2. 修改dispatcher检测的集合 -- 添加任务节点
            conn->m_evLoop->addTask(conn->m_channel, ElemType::MODIFY);
            if (conn->m_closeAfterWrite)
            {
                conn->m_evLoop->addTask(conn->m_channel, ElemType::DELETE);
            }
        }
    }
    return 0;
}

int TcpConnection::destroy(void *arg)
{
    TcpConnection *conn = static_cast<TcpConnection *>(arg);
    if (conn != nullptr)
    {
        delete conn;
    }
    return 0;
}

TcpConnection::TcpConnection(int fd, EventLoop *evloop, bool keepAlive, int keepAliveTimeout)
{
    m_evLoop = evloop;
    m_keepAlive = keepAlive;
    m_keepAliveTimeout = keepAliveTimeout;
    m_lastActiveTime = chrono::steady_clock::now();
    m_closeAfterWrite = false;
    m_readBuf = BufferPool::instance().acquire();
    m_writeBuf = BufferPool::instance().acquire();
    // http
    m_request = new HttpRequest;
    m_response = new HttpResponse;
    m_name = "Connection-" + to_string(fd);
    m_channel = new Channel(fd, FDEvent::ReadEvent, processRead, processWrite, destroy, this);
    if (m_keepAlive)
    {
        m_channel->timeoutCallback = processTimeout;
    }
    evloop->addTask(m_channel, ElemType::ADD);
}

TcpConnection::~TcpConnection()
{
    BufferPool::instance().release(m_readBuf);
    BufferPool::instance().release(m_writeBuf);
    delete m_request;
    delete m_response;
    m_evLoop->freeChannel(m_channel);
    Debug("连接断开, 释放资源, gameover, connName: %s", m_name.data());
}
