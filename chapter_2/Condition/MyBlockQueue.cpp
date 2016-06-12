#include <muduo/base/Thread.h>
#include <muduo/base/Condition.h>
#include <boost/bind.hpp>
#include <deque>

template<typename T>
class MyBlockQueue : boost::noncopyable {
public:
    MyBlockQueue() : mutex_(), cond_(mutex_), queue_() {  }

    T dequeue() {
        muduo::MutexLockGuard lock(mutex_); // 对条件变量进行加锁保护
        while (queue_.empty())   // 布尔表达式为queue.empty()
            cond_.wait();       // 等待原语为cond_wait()
        // 执行语句
        assert(!queue_.empty());
        T top = queue_.front();
        queue_.pop_front();
        return top;
    }

    void enqueue(const T &x) {
        muduo::MutexLockGuard lock(mutex_);
        queue_.push_back(x);
        cond_.notify();
    }

    std::deque<int>::size_type size() {
        muduo::MutexLockGuard lock(mutex_);
        std::deque<int>::size_type sz = queue_.size();
        return sz;
    }
private:
    muduo::MutexLock mutex_;
    muduo::Condition cond_;
    std::deque<T> queue_;
};

int main() {
    MyBlockQueue<int> mbq;
    muduo::Thread t1(boost::bind(&MyBlockQueue<int>::dequeue, &mbq));
    muduo::Thread t2(boost::bind(&MyBlockQueue<int>::enqueue, &mbq, 1));
    // 会造成死锁
    t1.start();     // 底层调用pthread_create()，表示开始线程
    t2.start();
    t1.join();      // 底层调用pthread_join()，表示以阻塞的方式等待线程的结束
    t2.join();
    return 0;
}
