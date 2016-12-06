//
// Created by dev on 16-12-3.
//

#ifndef RFTM_UI_TEST_H
#define RFTM_UI_TEST_H

#include <pthread.h>

#include "utils.h"
#include "ui_base.h"
#include "minui/minui.h"

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

    void start();

    void wait();

    int state() const { return state_; }
    const char* name() const { return name_; }
protected:
    void Draw();

    void OnKey(int value);

    void OnLeftTouch(int value);

    void OnRightTouch(int value);

    void OnAlarm();

    void OnEnter();

    void OnLeave();

    virtual void RunTest();
    inline void pass() { state_ = TS_PASSED; }
    inline void fail() { state_ = TS_FAILED; }

private:
    static void* do_test(void*);

    UiBase*     main_;
    int         state_;
    const char* name_;
    bool        started_;
    pthread_t   worker_;
};

#endif // RFTM_UI_TEST_H
