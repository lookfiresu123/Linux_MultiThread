#ifndef _H_COUNTER
#define _H_COUNTER 1

#include <boost/utility.hpp>
#include <muduo/base/CurrentThread.h>
#include <muduo/base/Mutex.h>
#include <iostream>
#include <memory>
#include <string>

// 计数器类
class Counter : boost::noncopyable {
public:
    // 友元函数
    friend void swap(std::shared_ptr<Counter> a, std::shared_ptr<Counter> b);
    friend void deadlock1(std::shared_ptr<Counter> a, std::shared_ptr<Counter> b);
    friend void deadlock2(std::shared_ptr<Counter> a, std::shared_ptr<Counter> b);

    // 默认构造函数
    Counter() : value_(0) {  }
    // 直接构造函数
    Counter(const std::string &_name) : thread_name(_name), value_(0) { }

    // 拷贝赋值运算符
    Counter &operator=(const Counter &rhs) {
        if (this == &rhs)
            return *this;
        muduo::MutexLockGuard mylock(mutex_);
        muduo::MutexLockGuard itslock(rhs.mutex_);
        value_ = rhs.value_;    // 改成value_ = rhs.value();会死锁，因为rhs.mutex_已经被占用
        return *this;
    }

    // 获得当前的计数值
    int64_t value() const {
        muduo::MutexLockGuard lock(mutex_);  // 占据互斥锁以防止其它线程或函数使用该对象
        return value_;
        // 在return语句执行完之后会释放互斥锁
    }

    // 获得当前的计数值并递增
    int64_t getAndIncrease() {
        muduo::MutexLockGuard lock(mutex_);
        int64_t ret = value_++;
        return ret;
    }

    // 获得当前的计数值并递减
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
