//
// Created by dev on 16-12-5.
//

#ifndef RFTM_BLOCKING_QUEUE_H
#define RFTM_BLOCKING_QUEUE_H

#include <queue>
#include <vector>
#include <pthread.h>

template <typename T>
class BlockingQueue
{
public:
    typedef T type;
    typedef size_t size_type;

    static const int kDefaultCapacity = 100;

    BlockingQueue() : BlockingQueue(kDefaultCapacity) {}

    BlockingQueue(size_type capacity) : capacity_(capacity) {
        pthread_mutex_init(&mutex_, NULL);
        pthread_cond_init(&cond_item_, NULL);
        pthread_cond_init(&cond_slot_, NULL);
    }

    void put(const T& item)
    {
        {
            ScopedLock _locker(&mutex_);
            // wait for a slot
            while (queue_.size() == capacity_) {
                pthread_cond_wait(&cond_slot_, &mutex_);
            }

            queue_.push(item);
        }
        // notify to a consumer, new item arrived.
        pthread_cond_signal(&cond_item_);
    }

    type take()
    {
        type result;
        {
            ScopedLock _locker(&mutex_);
            // wait for an item
            while (queue_.empty()) {
                pthread_cond_wait(&cond_item_, &mutex_);
            }

            result = queue_.front(); queue_.pop();
        }
        // notify to a producer, new slot available.
        pthread_cond_signal(&cond_slot_);
        return result;
    }

    std::vector<type> take_all()
    {
        std::vector<type> result;
        {
            ScopedLock _locker(&mutex_);

            // wait for an item
            while (queue_.empty()) {
                pthread_cond_wait(&cond_item_, &mutex_);
            }

            result.reserve(queue_.size());
            while (queue_.size() > 0) {
                result.push_back(queue_.front());
                queue_.pop();
            }
        }
        // notify to all producers, new slot available.
        pthread_cond_broadcast(&cond_slot_);
        return result;
    }

    // get all elements but not blocking
    std::vector<type> peek_all()
    {
        std::vector<type> result;
        {
            ScopedLock _locker(&mutex_);
            result.reserve(queue_.size());
            while (queue_.size() > 0) {
                result.push_back(queue_.front());
                queue_.pop();
            }
        }
        // notify to all producers, new slot available.
        pthread_cond_broadcast(&cond_slot_);
        return result;
    }

    size_type size() const
    {
        ScopedLock _locker(&mutex_);
        return queue_.size();
    }

    size_type capacity() const { return capacity_; }

    bool full() const
    {
        ScopedLock _locker(&mutex_);
        return queue_.size() == capacity_;
    }

    bool empty() const
    {
        ScopedLock _locker(&mutex_);
        return queue_.empty();
    }

protected:
    struct ScopedLock
    {
        ScopedLock(pthread_mutex_t* mutex) : mutex_(mutex) { pthread_mutex_lock(mutex_); }
        ~ScopedLock() { pthread_mutex_unlock(mutex_); }
    private:
        pthread_mutex_t* mutex_;
    };

private:
    size_type               capacity_;
    std::queue<type>        queue_;
    mutable pthread_mutex_t mutex_;
    pthread_cond_t          cond_item_;
    pthread_cond_t          cond_slot_;
};


#endif //RFTM_BLOCKING_QUEUE_H
