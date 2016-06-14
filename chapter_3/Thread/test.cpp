#include "Thread.h"
#include <boost/bind.hpp>

void test() {
    std::cout << "Thread ";
}

int main() {
    Thread t(boost::bind(test));
    t.start();
    t.join();
    return 0;
}
