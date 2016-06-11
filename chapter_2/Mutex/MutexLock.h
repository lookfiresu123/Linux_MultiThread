/* 对muduo库中的互斥锁类的重写 */

#ifndef _H_MUTEXLOCK
#define _H_MUTEXLOCK 1

#include <boost/utility.hpp>
#include <muduo/base/CurrentThread.h>
#include <pthread.h>
#include <cassert>
#include <system_error>
#include <iostream>

class MutexLock : boost::noncopyable {
public:
    // 异常处理函数
    static inline void __throw_system_error(int err_num) {
        try {
            throw std::system_error(err_num, std::system_category());
        } catch(const std::system_error &error) {
             std::cout << "Error : " << error.code() << " - " << error.code().message() << std::endl;
        }
    }

    // 互斥锁的构造函数
    MutexLock() : holder_(0) {
        pthread_mutex_init(&mutex_, NULL);
    }

    // 互斥锁的析构函数
    ~MutexLock() {
        assert(holder_ == 0);
        int __e = pthread_mutex_destroy(&mutex_);
        if (__e)
            __throw_system_error(__e);
    }

    // 每次加锁后
    bool isLockedByThisThread() const {
        return holder_ == muduo::CurrentThread::tid();
    }

    // 加锁，其中包含出错处理
    void lock() {
        int __e = pthread_mutex_lock(&mutex_);
        if (__e)
            __throw_system_error(__e);
        assignHolder();
    }

    // 解锁，其中包含出错处理
    void unlock() {
        int __e = pthread_mutex_unlock(&mutex_);
        if (__e)
            __throw_system_error(__e);
        unassignHolder();
    }

    // 获得互斥锁的地址
    pthread_mutex_t *getPthreadMutex() {
        return &mutex_;
    }

private:
    friend class Condition;

    // 私有内部类，负责守护工作
    class UnassignGuard : boost::noncopyable {
    public:
        UnassignGuard(MutexLock &owner) : owner_(owner) {
            owner_.unassignHolder();
        }

        ~UnassignGuard() {
            owner_.assignHolder();
        }
    private:
        MutexLock &owner_;  // 该内部类所对应的外部类对象，即拥有者（注意这是引用类型）
    };

    void assignHolder() {
        holder_ = muduo::CurrentThread::tid();
    }

    void unassignHolder() {
        holder_ = 0;
    }

    pid_t holder_;  // 线程号
    pthread_mutex_t mutex_; // 互斥锁
};

#endif
