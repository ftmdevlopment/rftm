//
// Created by dev on 16-12-3.
//

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "ui_test.h"
#include "minui/minui.h"
#include "ui_main.h"

#define msleep(n) usleep((n) * 1000)

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

void UiTest::OnKey(int code, int value)
{
    XLOGI("%s %d %d main: %p\n", __func__, code, value, main_);
//    UiBase::SetCurrentUI(main_);
}

void UiTest::OnLeftTouch(int value)
{
    XLOGI("%s %d %d", __func__, value, state());
    if (state() == TS_TESTING) return;
    if (in_left_filer(value)) return;
    fail();
    back();
}

void UiTest::OnRightTouch(int value)
{
    XLOGI("%s %d %d", __func__, value, state());
    if (state() == TS_TESTING) return;
    if (in_right_filter(value)) return;
    pass();
    back();
}

void UiTest::OnAlarm()
{
    XLOGI("last alarm: %.3f", get_last_alarm_ts());
    back();
}

void UiTest::OnEnter()
{
    XLOGI("enter %p start test", this);
    state(TS_TESTING);
    result("");
    start();
}

void UiTest::OnLeave()
{
    XLOGI("leave %p test", this);
    if (state() == TS_TESTING) {
        wait();
        result("");
        state(TS_INIT);
    }
}

void UiTest::RunTest()
{
    XLOGI("%p RunTest", this);
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
    pThis->back();
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

void UiTest::back()
{
    SetCurrentUI(main_);
}

void UiTest::clear_results()
{
    UiMain* main = dynamic_cast<UiMain*>(main_);
    if (main) {
        main->clear_results();
    }
}

bool UiTest::in_left_filer(int value)
{
    return false;
}

bool UiTest::in_right_filter(int value)
{
    return false;
}

void UiUserJudgeTest::clear_judge_result()
{
    judge_result_.clear();
}

bool UiUserJudgeTest::wait_for_judge_result()
{
    ready_ = true;

    bool result = judge_result_.take();
    if (result) {
        pass();
    } else {
        fail();
    }
    return result;
}

void UiUserJudgeTest::OnLeftTouch(int value)
{
    if (!ready_) return;
    if (in_left_filer(value)) return;
    UiTest::OnLeftTouch(value);
    judge_result_.put(false);
}

void UiUserJudgeTest::OnRightTouch(int value)
{
    if (!ready_) return;
    if (in_right_filter(value)) return;
    UiTest::OnRightTouch(value);
    judge_result_.put(true);
}

UiUserJudgeTest::UiUserJudgeTest(UiBase* main, const char* name)
        : UiTest(main, name)
{
}


