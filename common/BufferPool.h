#pragma once
#include <vector>
#include <mutex>
#include "Buffer.h"
using namespace std;
class BufferPool
{
public:
    // 获取单例实例
    static BufferPool &instance(int bufferSize = 4096, int poolSize = 100);

    Buffer *acquire();
    void release(Buffer *buffer);

private:
    BufferPool(int bufferSize, int poolSize);
    ~BufferPool();
    BufferPool(const BufferPool &) = delete;
    BufferPool &operator=(const BufferPool &) = delete;

    vector<Buffer *> m_pool;
    int m_bufferSize;
    mutex m_mutex;
};
