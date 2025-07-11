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

    // 新增 get/set 方法
    int getBufferSize() const { return m_bufferSize; }
    void setBufferSize(int size) { m_bufferSize = size; }
    int getPoolSize() const { return m_pool.size(); }
    vector<Buffer *> &getPool() { return m_pool; }

private:
    BufferPool(int bufferSize, int poolSize);
    ~BufferPool();
    BufferPool(const BufferPool &) = delete;
    BufferPool &operator=(const BufferPool &) = delete;

    vector<Buffer *> m_pool;
    int m_bufferSize;
    mutex m_mutex;
};
