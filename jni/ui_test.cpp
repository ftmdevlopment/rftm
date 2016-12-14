//
// Created by dev on 16-12-3.
//

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "ui_test.h"
#include "minui/minui.h"

#define msleep(n) usleep((n) * 1000)

struct ScopedLock
{
    ScopedLock(pthread_mutex_t* mutex) : mutex_(mutex) { pthread_mutex_lock(mutex_); }
    ~ScopedLock() { pthread_mutex_unlock(mutex_); }
private:
    pthread_mutex_t* mutex_;
};

UiTest::UiTest(UiBase* main, const char* name)
        : main_(main),
          state_(TS_INIT),
          name_(::strdup(name)),
          done_(false)
{
}

UiTest::~UiTest()
{
    wait();
    ::free((void*)name_);
}

void UiTest::Draw()
{
    if (main_) main_->Draw();
}

void UiTest::OnKey(int value)
{
    XLOGI("%s %d main: %p\n", __func__, value, main_);
//    UiBase::SetCurrentUI(main_);
}

void UiTest::OnLeftTouch(int value)
{
    XLOGI("%s %d\n", __func__, value);
    if (value == 0x10 || value == 0x08) {
        fail();
        SetCurrentUI(main_);
    }
}

void UiTest::OnRightTouch(int value)
{
    XLOGI("%s %d\n", __func__, value);
    if (value == 0x10 || value == 0x08) {
        pass();
        SetCurrentUI(main_);
    }
}

void UiTest::OnAlarm()
{
    XLOGI("alarm");
    UiBase::SetCurrentUI(main_);
}

void UiTest::OnEnter()
{
    set_alarm(10);
    XLOGI("enter %p start test", this);
    state(TS_TESTING);
    result("");
    start();
}

void UiTest::OnLeave()
{
    XLOGI("leave %p wait test finish", this);
    if (state() == TS_TESTING) {
        wait();
        result("");
        state(TS_INIT);
    }
}

void UiTest::RunTest()
{
}

const char* UiTest::state_str() const
{
    const char* names[] = {
            [TS_INIT] = "",
            [TS_TESTING] = "Testing",
            [TS_PASSED] = "Passed",
            [TS_FAILED] = "Failed"
    };
    int s = state();
    if (s >= sizeof(names)/sizeof(names[0]) || s < 0) s = 0;
    return names[s];
}

int UiTest::state() const
{
    ScopedLock _locker(&mutex_);
    return state_;
}

void UiTest::state(int s)
{
    ScopedLock _locker(&mutex_);
    state_ = s;
}

std::string UiTest::result() const
{
    ScopedLock _locker(&mutex_);
    return result_;
}

void UiTest::result(std::string r)
{
    ScopedLock _locker(&mutex_);
    result_ = r;
}

void* UiTest::do_test(void *thiz)
{
    UiTest* pThis = (UiTest*)thiz;
    pThis->RunTest();
    return NULL;
}

void UiTest::start()
{
    done_ = false;
    pthread_create(&worker_, NULL, do_test, this);
}

void UiTest::wait()
{
    pthread_join(worker_, NULL);
    done_ = true;
}
