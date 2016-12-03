//
// Created by dev on 16-12-3.
//

#ifndef RFTM_THREAD_H
#define RFTM_THREAD_H


class thread
{
public:
    void start();
    void join();

protected:
    virtual void run() = 0;

private:
    static void* entry(void*);
};


#endif //RFTM_THREAD_H
