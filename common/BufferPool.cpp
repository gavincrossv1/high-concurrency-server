#include "BufferPool.h"

// 单例获取
BufferPool &BufferPool::instance(int bufferSize, int poolSize)
{
    static BufferPool pool(bufferSize, poolSize);
    return pool;
}

// 构造函数，预分配 poolSize 个 Buffer
// 一次性分配连续内存
BufferPool::BufferPool(int bufferSize, int poolSize)
    : m_bufferSize(bufferSize)
{
    // 一次性分配所有Buffer所需的内存
    char *bulkMemory = new char[poolSize * (sizeof(Buffer) + bufferSize)];

    for (int i = 0; i < poolSize; ++i)
    {
        // 使用placement new在预分配的内存上构造Buffer
        m_pool.push_back(new (bulkMemory + i * (sizeof(Buffer) + bufferSize))
                             Buffer(bufferSize));
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
Buffer *BufferPool::acquire()
{
    lock_guard<mutex> lock(m_mutex); // 加锁保证线程安全
    if (!m_pool.empty())
    {
        Buffer *buf = m_pool.back(); // 从池中取出最后一个Buffer
        m_pool.pop_back();
        buf->reset();
        return buf;
    }
    return new Buffer(m_bufferSize); // 如果池为空，新建一个Buffer
}

// 归还一个 Buffer 对象（线程安全）
void BufferPool::release(Buffer *buffer)
{
    if (buffer) // 检查指针有效性
    {
        lock_guard<mutex> lock(m_mutex); // 加锁保证线程安全
        buffer->reset();
        m_pool.push_back(buffer);                                   // 将Buffer放回池中
    }
}