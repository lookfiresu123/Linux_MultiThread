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
    Counter(const std::string &_name) : thread_name(_name), value_(0) { }

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
private:
    std::string thread_name;
    int64_t value_;
    mutable muduo::MutexLock mutex_;
};

extern void swap(std::shared_ptr<Counter> a, std::shared_ptr<Counter> b);
extern void deadlock1(std::shared_ptr<Counter> a, std::shared_ptr<Counter> b);
extern void deadlock2(std::shared_ptr<Counter> a, std::shared_ptr<Counter> b);

#endif
