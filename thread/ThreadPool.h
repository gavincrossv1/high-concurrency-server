#pragma once
#include "EventLoop.h"
#include <stdbool.h>
#include "WorkerThread.h"
#include <vector>
using namespace std;

// 定义线程池
class ThreadPool
{
public:
    ThreadPool(EventLoop *mainLoop, int count);
    ~ThreadPool();
    // 启动线程池
    void run();
    // 取出线程池中的某个子线程的反应堆实例
    EventLoop *takeWorkerEventLoop();
    // 新增 get/set 方法
    inline EventLoop *getMainLoop() { return m_mainLoop; }
    inline bool isStarted() { return m_isStart; }
    inline int getThreadNum() { return m_threadNum; }
    inline int getIndex() { return m_index; }
    inline void setIndex(int idx) { m_index = idx; }
    inline std::vector<WorkerThread *> &getWorkerThreads() { return m_workerThreads; }

private:
    // 主线程的反应堆模型
    EventLoop *m_mainLoop;
    bool m_isStart;
    int m_threadNum;
    vector<WorkerThread *> m_workerThreads;
    int m_index;
};