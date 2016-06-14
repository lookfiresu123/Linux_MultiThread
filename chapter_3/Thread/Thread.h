#ifndef _H_THREAD
#define _H_THREAD 1

#include <boost/utility.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <pthread.h>
#include <string>
#include <cassert>
#include <iostream>


void *startThread(void *) {
    std::cout << "create success" << std::endl;
    return NULL;
}

class Thread : boost::noncopyable {
public:
    typedef boost::function<void ()> ThreadFunc;

    explicit Thread(const ThreadFunc &func, const std::string &name = std::string())
        : started_(false), joined_(false), pthreadId_(0), tid_(new pid_t(0)), func_(func), name_(name)
    {
        func_();
        setDefaultName();
    }

    ~Thread() {
        if (started_ && !joined_)
            pthread_detach(pthreadId_);
    }

    void start() {
        assert(!started_);
        started_ = true;
        if (pthread_create(&pthreadId_, NULL, startThread, NULL)) {
            started_ = false;
            std::cout << "Fail in pthread_create" << std::endl;
        }
    }

    int join() {
        assert(started_);
        assert(!joined_);
        joined_ = true;
        return pthread_join(pthreadId_, NULL);
    }

    bool started() const { return started_; }
    pid_t tid() const { return *tid_; }
    const std::string &name() const { return name_; }

private:
    void setDefaultName() {
         if (name_.empty())
             name_ = "Thread" + std::to_string(++numCreated_);
    }

    bool started_;
    bool joined_;
    pthread_t pthreadId_;
    boost::shared_ptr<pid_t> tid_;
    ThreadFunc func_;
    std::string name_;
    static unsigned int numCreated_;
};

unsigned int Thread::numCreated_ = 0;

#endif
