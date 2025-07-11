#pragma once
#include "Channel.h"
#include "EventLoop.h"
#include "Dispatcher.h"
#include <string>
#include <sys/select.h>
using namespace std;

class SelectDispatcher : public Dispatcher
{
public:
    SelectDispatcher(EventLoop *evloop);
    ~SelectDispatcher();
    // 添加
    int add() override;
    // 删除
    int remove() override;
    // 修改
    int modify() override;
    // 事件监测
    int dispatch(int timeout = 2) override; // 单位: s

    // 获取读集合
    inline fd_set getReadSet() const { return m_readSet; }
    // 获取写集合
    inline fd_set getWriteSet() const { return m_writeSet; }
    // 获取最大尺寸
    inline int getMaxSize() const { return m_maxSize; }
    // 清空所有 fd_set
    void resetFdSets()
    {
        FD_ZERO(&m_readSet);
        FD_ZERO(&m_writeSet);
    }

private:
    void setFdSet();
    void clearFdSet();

private:
    fd_set m_readSet;
    fd_set m_writeSet;
    const int m_maxSize = 1024;
};