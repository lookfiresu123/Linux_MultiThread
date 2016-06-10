#ifndef _H_COUNTER
#define _H_COUNTER 1

#include <boost/utility.hpp>
#include <muduo/base/CurrentThread.h>
#include <muduo/base/Mutex.h>
#include <iostream>
#include <memory>
#include <string>


class Counter : boost::noncopyable {
public:
    friend void swap(std::shared_ptr<Counter> a, std::shared_ptr<Counter> b);
    friend void deadlock1(std::shared_ptr<Counter> a, std::shared_ptr<Counter> b);
    friend void deadlock2(std::shared_ptr<Counter> a, std::shared_ptr<Counter> b);

    // construct function
    Counter() : value_(0) {  }
    Counter(const std::string &_name) : thread_name(_name), value_(0) { }

    Counter &operator=(const Counter &rhs) {
        if (this == &rhs)
            return *this;
        muduo::MutexLockGuard mylock(mutex_);
        muduo::MutexLockGuard itslock(rhs.mutex_);
        value_ = rhs.value_;    // 改成value_ = rhs.value();会死锁，因为rhs.mutex_已经被占用
        return *this;
    }

    // get the current value
    int64_t value() const {
        muduo::MutexLockGuard lock(mutex_);  // get the mutex and lock itself, so that other threads can not implement this object
        return value_;
        // the mutex_ will be unlock after return instruction
    }

    // get the current value and add itself
    int64_t getAndIncrease() {
        muduo::MutexLockGuard lock(mutex_);
        int64_t ret = value_++;
        return ret;
    }

    // get the current value and sub itself
    int64_t getAndDecrease() {
        muduo::MutexLockGuard lock(mutex_);
        int64_t ret = value_++;
        return ret;
    }
protected:
    std::string thread_name;
    int64_t value_;
    mutable muduo::MutexLock mutex_;
};

extern void swap(std::shared_ptr<Counter> a, std::shared_ptr<Counter> b);
extern void deadlock1(std::shared_ptr<Counter> a, std::shared_ptr<Counter> b);
extern void deadlock2(std::shared_ptr<Counter> a, std::shared_ptr<Counter> b);

#endif
