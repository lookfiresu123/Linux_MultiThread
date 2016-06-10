// clang++ -std=c++11 test.cpp /usr/lib/libmuduo_base_cpp11.a -lpthread -o main

#include <muduo/base/Thread.h>
#include <stdio.h>
#include "Counter.h"
#include "boost/bind.hpp"
#include <vector>
#include <memory>

void threadFunc(int val) {
    printf("pid = %d, tid = %d, value = %d\n", ::getpid(), muduo::CurrentThread::tid(), val);
}

int counter_test() {
    Counter my_counter;
    threadFunc(my_counter.getAndIncrease());

    std::vector<std::shared_ptr<muduo::Thread> > threads;
    // create a thread and start it
    for (int i = 0 ; i < 3 ; ++i) {
        std::shared_ptr<muduo::Thread> p(new muduo::Thread(boost::bind(threadFunc, my_counter.getAndIncrease())));
        threads.push_back(p);
        p->start();
        p->join();
    }

    // muduo::Thread t1(boost::bind(threadFunc, my_counter.getAndIncrease()));
    // t1.start();
    // t1.join();
    return 0;
}

int main() {
    counter_test();
    return 0;
}
