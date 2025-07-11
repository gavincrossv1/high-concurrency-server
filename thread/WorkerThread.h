#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include "EventLoop.h"
using namespace std;

// 定义子线程对应的结构体
class WorkerThread
{
public:
    WorkerThread(int index);
    ~WorkerThread();
    // 启动线程
    void run();
    inline EventLoop *getEventLoop()
    {
        return m_evLoop;
    }
    // 新增 get/set 方法
    inline std::thread *getThread() { return m_thread; }
    inline std::thread::id getThreadID() { return m_threadID; }
    inline std::string getName() { return m_name; }
    inline void setName(const std::string &name) { m_name = name; }
    inline void setThreadID(const std::thread::id &id) { m_threadID = id; }
    inline void setEventLoop(EventLoop *loop) { m_evLoop = loop; }

private:
    void running();

private:
    thread *m_thread;      // 保存线程的实例
    thread::id m_threadID; // ID
    string m_name;
    mutex m_mutex;             // 互斥锁
    condition_variable m_cond; // 条件变量
    EventLoop *m_evLoop;       // 反应堆模型
};
