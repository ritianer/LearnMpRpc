#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class LockQuene
{
public:
    void Push(const T &data)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_quene.push(data);
        m_condvar.notify_one();
    }
    T Pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while(m_quene.empty())
        {
            m_condvar.wait(lock); // 阻塞等待pop线程notify_one
        }
        T data = m_quene.front();
        m_quene.pop();
        return data;
    }

private:
    std::mutex m_mutex;
    std::queue<T> m_quene;
    std::condition_variable m_condvar;
};