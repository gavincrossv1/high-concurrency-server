#pragma once
#include "Channel.h"
#include "EventLoop.h"
#include <string>
using namespace std;
class EventLoop;
class Dispatcher
{
public:
    Dispatcher(EventLoop *evloop);
    virtual ~Dispatcher();
    // 添加
    virtual int add();
    // 删除
    virtual int remove();
    // 修改
    virtual int modify();
    // 事件监测
    virtual int dispatch(int timeout = 2); // 单位: s
    inline void setChannel(Channel *channel)
    {
        m_channel = channel;
    }
    // 获取当前 Channel
    inline Channel *getChannel() const
    {
        return m_channel;
    }
    // 获取 EventLoop
    inline EventLoop *getEventLoop() const
    {
        return m_evLoop;
    }
    // 获取/设置名称
    inline string getName() const
    {
        return m_name;
    }
    inline void setName(const string &name)
    {
        m_name = name;
    }

protected:
    string m_name = string();
    Channel *m_channel;
    EventLoop *m_evLoop;
};