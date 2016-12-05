//
// Created by dev on 16-12-3.
//

#ifndef RFTM_UI_TEST_H
#define RFTM_UI_TEST_H

#include <pthread.h>

#include <stdio.h>
#include <string.h>

#include "ui_base.h"

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

    inline int get_status() const { return status_; }
protected:
    void Draw();

    void OnKey(int value);

    void OnLeftTouch(int value);

    void OnRightTouch(int value);

    void OnAlarm();

    virtual void RunTest();
    inline void pass() { status_ = TS_PASSED; }
    inline void fail() { status_ = TS_FAILED; }

private:
    static void* do_test(void*);

    pthread_t worker_;
    UiBase*   ui_main_;
    int       status_;
    const char* name_;
};

#endif // RFTM_UI_TEST_H
