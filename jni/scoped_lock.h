//
// Created by dev on 16-12-19.
//

#ifndef RFTM_SCOPED_LOCK_H
#define RFTM_SCOPED_LOCK_H

#include <pthread.h>

class ScopedLock
{
public:
    ScopedLock(pthread_mutex_t* mutex) : mutex_(mutex) { pthread_mutex_lock(mutex_); }
    ~ScopedLock() { pthread_mutex_unlock(mutex_); }
private:
    pthread_mutex_t* mutex_;
};

#endif //RFTM_SCOPED_LOCK_H
