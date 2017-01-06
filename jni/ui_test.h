//
// Created by dev on 16-12-3.
//

#ifndef RFTM_UI_TEST_H
#define RFTM_UI_TEST_H

#include <pthread.h>
#include <string>

#include "log.h"
#include "utils.h"
#include "common.h"
#include "ui_base.h"
#include "minui/minui.h"
#include "blocking_queue.h"

class UiTest : public UiBase
{
public:
    UiTest(UiBase* main, const char* name);

    UiTest(UiBase* main) : UiTest(main, "UiTest") {}

    UiTest() : UiTest(this) {}

    ~UiTest();

    enum TestState {
        TS_INIT,
        TS_TESTING,
        TS_PASSED,
        TS_FAILED,
    };

    const char* name() const { return name_; }
    const char* state_str() const;
    int state() const;
    void state(int s);
    std::string result() const;
    void result(std::string r);
protected:
    void Draw();

    void OnKey(int code, int value);

    void OnLeftTouch(int value);

    void OnRightTouch(int value);

    bool in_left_filer(int value);
    bool in_right_filter(int value);

    void OnAlarm();

    void OnEnter();

    void OnLeave();

    virtual void RunTest();
    inline void pass() { state(TS_PASSED); }
    inline void fail() { state(TS_FAILED); }

    void start();
    void wait();

    void back();

    void clear_results();

private:
    static void* do_test(void*);

    UiBase*     main_;
    int         state_;
    const char* name_;
    std::string result_;
    bool        done_;
    pthread_t   worker_;
    mutable pthread_mutex_t mutex_;
};

#define UITEST_ENTRY(Class) \
    Class(UiBase* main, const char* name) : UiTest(main, name)

class UiAutoJudgeTest : public UiTest
{
public:
    UITEST_ENTRY(UiAutoJudgeTest) {}

    void OnKey(int code, int value) {}

    void OnLeftTouch(int value) {}

    void OnRightTouch(int value) {}
};

#define AUTO_JUDGE_TEST_ENTRY(Class) \
    Class(UiBase* main, const char* name) : UiAutoJudgeTest(main, name)

class UiUserJudgeTest : public UiTest
{
protected:
    UiUserJudgeTest(UiBase* main, const char* name);

    void clear_judge_result();

    void wait_for_judge_result();

    void OnLeftTouch(int value);

    void OnRightTouch(int value);

private:
    volatile bool ready_ = false;
    BlockingQueue<bool> judge_result_;
};

#define USER_JUDGE_TEST_ENTRY(Class) \
    Class(UiBase* main, const char* name) : UiUserJudgeTest(main, name)

#endif // RFTM_UI_TEST_H
