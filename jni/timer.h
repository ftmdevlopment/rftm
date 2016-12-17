//
// Created by dev on 16-12-17.
//

#ifndef RFTM_TIMER_H
#define RFTM_TIMER_H

#include <time.h>

class Timer
{
public:
    Timer() { reset(); }

    double elapsed() {
        clock_gettime(CLOCK_REALTIME, &ts_end_);
        return tstod(&ts_end_) - tstod(&ts_start_);
    }

    void reset() {
        clock_gettime(CLOCK_REALTIME, &ts_start_);
    }
private:
    static double tstod(struct timespec* ts) {
        return ts->tv_sec + ts->tv_nsec/1e9;
    }

    struct timespec ts_start_;
    struct timespec ts_end_;
};

#endif //RFTM_TIMER_H
