#include <vector>
#include <muduo/base/Mutex.h>

class Foo {
public:
    Foo(int data) : data_(data) {  }
    ~Foo() = default;

private:
    int data_;
};

class Mutex_Test {
public:
    std::vector<Foo>::iterator myErase(std::vector<Foo>::iterator it) {
        muduo::MutexLockGuard lock(mutex_);
        return foos.erase(it);
    }

    void traverse() {
        muduo::MutexLockGuard lock(mutex_);
        for (std::vector<Foo>::iterator it = foos.begin() ; it != foos.end() ; )
            it = myErase(it);
    }

    void myPush(const Foo &f) {
        foos.push_back(f);
    }

    int test() {
        traverse();
        return 0;
    }
private:
    muduo::MutexLock mutex_;
    std::vector<Foo> foos;
};

int main() {
    Mutex_Test per;
    for (int i = 0 ; i < 10 ; ++i)
        per.myPush(Foo(i));

    // 由于mutex_是非递归互斥锁，因此同一个线程重复（递归）加锁会造成死锁
    per.test();
    return 0;
}
