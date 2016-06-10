#ifndef _H_REFERENCE_COUNT
#define _H_REFERENCE_COUNT 1

#include "../Counter/Counter.h"

// 互斥锁无法保护析构，因为析构过程中互斥锁会被销毁，而若其它线程正在等待互斥锁，那些线程会被永久阻塞
class Reference_Counter;

// 对象类
// 除了该对象所对应的那个引用计数外，任何其它对象都不能接触该对象，否则会造成析构的安全问题
class Object {
public:
    // 直接构造函数
    Object(int &data) : data_(data) {  }
    // 拷贝构造函数
    Object(const Object &x) : data_(x.data_) {  }

    // 析构函数
    virtual ~Object() {}

    // 重载赋值运算符
    Object &operator=(Object &rhs) {
        muduo::MutexLockGuard lock(mutex_);
        data_ = rhs.data_;
        return *this;
    }

private:
    mutable muduo::MutexLock mutex_;    // 互斥器
    int data_;  // 数据
};

// 引用计数类
class Reference_Counter : public Counter {
public:
    // 直接构造函数
    Reference_Counter(int data) : Counter(), ptr_(nullptr) {
        if (ptr_ == nullptr) {
            muduo::MutexLockGuard lock(mutex_);
            ptr_ = new Object(data);
        }
    }

    // 拷贝构造函数
    Reference_Counter(const Reference_Counter &x) : ptr_(x.ptr_) {  }

    // 执行析构函数时，假设没有任何其它智能指针在使用该引用计数对象
    ~Reference_Counter() {
        delete ptr_;
        ptr_ = nullptr;
    }

    // 获得当前引用计数值
    int64_t get_reference_counter() {
        muduo::MutexLockGuard lock(mutex_);
        return value_;
    }

private:
    Object *ptr_;   // 指向实际对象的指针

    // 获得所指对象地址
    Object *get_object_ptr() {
        muduo::MutexLockGuard lock(mutex_);
        return ptr_;
    }

};



// 智能指针类
class Smart_Pointer {
public:
    // 直接构造函数
    Smart_Pointer(int &data) : ptr_(nullptr) {
        if (ptr_ == nullptr) {
            muduo::MutexLockGuard lock(mutex_);
            ptr_ = new Reference_Counter(data);
        }
    }

    // 智能指针的拷贝构造函数
    Smart_Pointer(const Smart_Pointer &x) : ptr_(x.ptr_) {  }

    // 执行析构函数时，假设没有任何其它线程在使用该智能指针
    ~Smart_Pointer() {
        ptr_->getAndDecrease();
        if (ptr_->get_reference_counter() == 0) {
            delete ptr_;
            ptr_ = nullptr;
        }
    }
private:
    mutable muduo::MutexLock mutex_;    // 互斥器
    Reference_Counter *ptr_;    // 指向引用计数模块的指针
};

#endif
