#include "BufferPool.h"

// 单例获取
BufferPool& BufferPool::instance(int bufferSize, int poolSize)
{
    static BufferPool pool(bufferSize, poolSize);
    return pool;
}

// 构造函数，预分配 poolSize 个 Buffer
BufferPool::BufferPool(int bufferSize, int poolSize)
    : m_bufferSize(bufferSize)
{
    for (int i = 0; i < poolSize; ++i)
    {
        m_pool.push_back(new Buffer(bufferSize));
    }
}

BufferPool::~BufferPool()
{
    for (size_t i = 0; i < m_pool.size(); ++i)
    {
        delete m_pool[i];
    }
    m_pool.clear();
}

// 获取一个 Buffer 对象（线程安全）
Buffer* BufferPool::acquire()
{
    lock_guard<mutex> lock(m_mutex);
    if (!m_pool.empty())
    {
        Buffer* buf = m_pool.back();
        m_pool.pop_back();
        buf->readPosIncrease(-buf->readableSize());
        buf->extendRoom(m_bufferSize - buf->writeableSize());
        return buf;
    }
    return new Buffer(m_bufferSize);
}

// 归还一个 Buffer 对象（线程安全）
void BufferPool::release(Buffer* buffer)
{
    if (buffer)
    {
        lock_guard<mutex> lock(m_mutex);
        buffer->readPosIncrease(-buffer->readableSize());
        buffer->extendRoom(m_bufferSize - buffer->writeableSize());
        m_pool.push_back(buffer);
    }
}
