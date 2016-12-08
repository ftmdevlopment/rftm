//
// Created by dev on 16-12-3.
//

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "ui_test.h"
#include "minui/minui.h"

#define msleep(n) usleep((n) * 1000)

struct SocpedLock
{
    SocpedLock(pthread_mutex_t* mutex) : mutex_(mutex) { pthread_mutex_lock(mutex_); }
    ~SocpedLock() { pthread_mutex_unlock(mutex_); }
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
    gr_info("%s %d main: %p\n", __func__, value, main_);
//    UiBase::SetCurrentUI(main_);
}

void UiTest::OnLeftTouch(int value)
{
    gr_info("%s %d\n", __func__, value);
    fail();
    SetCurrentUI(main_);
}

void UiTest::OnRightTouch(int value)
{
    gr_info("%s %d\n", __func__, value);
    pass();
    SetCurrentUI(main_);
}

void UiTest::OnAlarm()
{
    gr_info("alarm");
    UiBase::SetCurrentUI(main_);
}

void UiTest::OnEnter()
{
    set_alarm(10);
    gr_info("enter %p start test", this);
    state(TS_TESTING);
    result("");
    start();
}

void UiTest::OnLeave()
{
    gr_info("leave %p wait test finish", this);
    if (state() == TS_TESTING) {
        wait();
        result("");
        state(TS_INIT);
    }
}

void UiTest::RunTest()
{
    gr_info("Running");
    result("UiTest::RunTest\ndone\n3\n4\n5\n6\n7");
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
    SocpedLock _locker(&mutex_);
    return state_;
}

void UiTest::state(int s)
{
    SocpedLock _locker(&mutex_);
    state_ = s;
}

std::string UiTest::result() const
{
    SocpedLock _locker(&mutex_);
    return result_;
}

void UiTest::result(std::string r)
{
    SocpedLock _locker(&mutex_);
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
