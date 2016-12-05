//
// Created by dev on 16-12-5.
//
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>

#include <thread>

#include "blocking_queue.h"

#ifdef _GNU_SOURCE
pid_t gettid() { return syscall(SYS_gettid); }
#endif

static int __single_producer_single_consumer_test__ = [](){
    BlockingQueue<int> queue;

    printf("queue size: %d\n", queue.size());
    printf("queue full: %s\n", queue.full() ? "true" : "false");
    printf("queue empty: %s\n", queue.empty() ? "true" : "false");
    printf("queue capacity: %d\n", queue.capacity());

    const int kTestSize = queue.kDefaultCapacity * 2;

    std::thread producer([&](){
        for (int i = 0; i < kTestSize; i++) {
            printf("[%d] I want to put %d to queue...\n", gettid(), i);
            queue.put(i);
            printf("[%d] put %d done\n", gettid(), i);
        }
    });

    usleep(200*1000);
    puts("===========================================================");
    printf("queue size: %d\n", queue.size());
    printf("queue full: %s\n", queue.full() ? "true" : "false");
    printf("queue empty: %s\n", queue.empty() ? "true" : "false");
    printf("queue capacity: %d\n", queue.capacity());
    puts("===========================================================");
    usleep(200*1000);

    std::thread consumer([&](){
        for (int i = 0; i < kTestSize; i++) {
            printf("[%d] I will take(%d) an item from queue\n", gettid(), i);
            int item = queue.take();
            printf("[%d] take(%d) done, item: %d\n", gettid(), i, item);
        }
    });

    producer.join();
    consumer.join();

    return 0;
}();

#ifdef _GNU_SOURCE
int main()
{
    return 0;
}
#endif