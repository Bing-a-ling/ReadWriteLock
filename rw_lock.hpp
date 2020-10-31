// File: rw_lock.hpp
// Author: Bing-a-ling
// This head file implements a write-first read&write lock. It allows only one thread to write at one time. It allows multiple threads to read at the same time when there is no writing going on.
#pragma once s
#include <mutex>
#include <condition_variable>

class ReadWriteLock{
public:
    ReadWriteLock()
    :m_read_count_(0),
    m_write_count_(0){}
    virtual ~ReadWriteLock() = default;
    void lockWrite(){
        std::unique_lock<std::mutex> guard(m_lock_);
        ++m_write_count_;
        m_write_condition_.wait(guard, [=]{
            return m_read_count_ == 0 && !m_is_writing_;
        });
        m_is_writing_ = true;
    }
    void lockRead(){
        std::unique_lock<std::mutex> guard(m_lock_);
        m_read_condition_.wait(guard, [=]{
            return m_write_count_ == 0;
        });
        ++m_read_count_;
    }
    void unlockWrite(){
        std::unique_lock<std::mutex> guard(m_lock_);
        m_is_writing_ = false;
        if(--m_write_count_ == 0){
            m_read_condition_.notify_all();
        } else {
            m_write_condition_.notify_one();
        }

    }
    void unlockRead(){
        std::unique_lock<std::mutex> guard(m_lock_);
        if(--m_read_count_ == 0 && m_write_count_ > 0){
            m_write_condition_.notify_one();
        }
    }
private:
    volatile int m_read_count_;
    volatile int m_write_count_;
    volatile bool m_is_writing_;
    std::mutex m_lock_;
    std::condition_variable m_read_condition_;
    std::condition_variable m_write_condition_;
};

class WriteLock{
public:
    WriteLock(ReadWriteLock& lock)
    :m_lock_(lock){
        m_lock_.lockWrite();
    }
    virtual ~WriteLock(){
        m_lock_.unlockWrite();
    }
private:
    ReadWriteLock& m_lock_;
};

class ReadLock{
public:
    ReadLock(ReadWriteLock& lock)
    :m_lock_(lock){
        m_lock_.lockRead();
    }
    virtual ~ReadLock(){
        m_lock_.unlockRead();
    }
private:
    ReadWriteLock& m_lock_;
};

