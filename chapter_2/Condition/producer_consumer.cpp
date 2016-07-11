#include <iostream>
#include <pthread.h>

using namespace std;

static const int MAX = 100;
static pthread_mutex_t the_mutex;
static pthread_cond_t condc, condp;
static int buffer = 0;

void *producer(void *ptr) {
    for (int i = 1 ; i <= MAX ; ++i) {
        pthread_mutex_lock(&the_mutex);             // 加锁
        while (buffer != 0)
            pthread_cond_wait(&condp, &the_mutex);  // 阻塞生产者，并释放the_mutex
        buffer = i;
        cout << "producer " << buffer << endl;
        pthread_cond_signal(&condc);                // 唤醒消费者
        pthread_mutex_unlock(&the_mutex);           // 解锁
    }
    pthread_exit(0);
}

void *consumer(void *ptr) {
    for (int i = 1 ; i <= MAX ; ++i) {
        pthread_mutex_lock(&the_mutex);             // 加锁
        while (buffer == 0)
            pthread_cond_wait(&condc, &the_mutex);  // 阻塞消费者，并释放the_mutex
        buffer = 0;
        cout << "consumer " << buffer << endl;
        pthread_cond_signal(&condp);                // 唤醒生产者
        pthread_mutex_unlock(&the_mutex);           // 解锁
    }
    pthread_exit(0);
}

int main() {
    pthread_t pro, con;
    pthread_mutex_init(&the_mutex, 0);              // 设置the_mutex初始值为0，其中0表示解锁状态，1表示加锁状态
    pthread_cond_init(&condc, 0);
    pthread_cond_init(&condp, 0);
    pthread_create(&con, 0, consumer, 0);
    pthread_create(&pro, 0, producer, 0);
    pthread_join(pro, 0);
    pthread_join(con, 0);
    pthread_cond_destroy(&condc);
    pthread_cond_destroy(&condp);
    pthread_mutex_destroy(&the_mutex);
    return 0;
}
