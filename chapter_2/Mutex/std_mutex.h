/* 对c++标准中的互斥锁类的重写 */

#ifndef _H_STD_MUTEX
#define _H_STD_MUTEX 1

#include <pthread.h>
#include <system_error>
#include <iostream>

class __mutex_base {
protected:
    pthread_mutex_t _M_mutex = PTHREAD_MUTEX_INITIALIZER;

    // 使用编译器合成的默认构造函数，构造函数执行过程中不允许抛出异常
    constexpr __mutex_base() noexcept = default;

    //销毁互斥锁，执行过程中不允许抛出异常
    ~__mutex_base() noexcept {
        pthread_mutex_destroy(&_M_mutex);
    }

    // 不能拷贝构造互斥锁
    __mutex_base(const __mutex_base &) = delete;

    // 不能拷贝赋值互斥锁
    __mutex_base &operator=(const __mutex_base) = delete;
};

class mutex : private __mutex_base {
public:
    // 互斥锁的构造函数和析构函数
    mutex() noexcept = default;
    ~mutex() = default;
    mutex(const mutex &) = default;
    mutex &operator=(const mutex &) = delete;

    // 异常处理函数
    static inline void __throw_system_error(int err_num) {
        try {
            throw std::system_error(err_num, std::system_category());
        } catch(const std::system_error &error) {
             std::cout << "Error : " << error.code() << " - " << error.code().message() << std::endl;
        }
    }

    // 加锁，底层使用c库中的函数来加锁
    void lock() {
        int __e = pthread_mutex_lock(&_M_mutex);
        if (__e)
            __throw_system_error(__e);
    }

    // 加锁并判断是否成功
    bool try_lock() noexcept {
        return !pthread_mutex_trylock(&_M_mutex);
    }

    // 解锁，底层使用c库中的函数来解锁
    void unlock() {
        pthread_mutex_unlock(&_M_mutex);
    }

    // 返回互斥锁的地址
    pthread_mutex_t *native_handle() {
        return &_M_mutex;
    }
};

#endif
