#include "../Counter/Counter.h"
#include <muduo/base/Thread.h>
#include <boost/bind.hpp>

void swap(std::shared_ptr<Counter> a, std::shared_ptr<Counter> b) {
    muduo::MutexLockGuard alock(a->mutex_);
    muduo::MutexLockGuard block(b->mutex_);
    int64_t value = a->value_;
    a->value_ = b->value_;
    b->value_ = value;
    printf("pid = %d, tid = %d, %s->value_ = %d, %s->value_ = %d\n",
            ::getpid(), muduo::CurrentThread::tid(), a->thread_name.c_str(), static_cast<int>(a->value_), b->thread_name.c_str(), static_cast<int>(b->value_));
}

void deadlock1(std::shared_ptr<Counter> a, std::shared_ptr<Counter> b) {
    muduo::MutexLockGuard alock(a->mutex_);
    muduo::Thread t2(boost::bind(deadlock2, b, a));
    t2.start();
    t2.join();
    muduo::MutexLockGuard block(b->mutex_);
}

void deadlock2(std::shared_ptr<Counter> a, std::shared_ptr<Counter> b) {
    muduo::MutexLockGuard alock(a->mutex_);
    muduo::MutexLockGuard block(b->mutex_); // 此时a->mutex_被线程t1占据，从而造成死锁
}

int DeadLock_Test() {
    std::shared_ptr<Counter> a(new Counter("a"));
    std::shared_ptr<Counter> b(new Counter("b"));
    b->getAndIncrease();    // a->value_ = 0, b->value_ = 1
    // muduo::Thread ta(boost::bind(swap, a, b));
    // muduo::Thread tb(boost::bind(swap, b, a));
    muduo::Thread t1(boost::bind(deadlock1, a, b));
    t1.start();
    t1.join();
    // ta.start();
    // tb.start();
    // tb.join();
    // ta.join();
    return 0;
}

int main() {
    DeadLock_Test();
    return 0;
}
