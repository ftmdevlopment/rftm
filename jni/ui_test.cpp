//
// Created by dev on 16-12-3.
//

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "ui_test.h"
#include "minui/minui.h"

#define msleep(n) usleep((n) * 1000)

UiTest::UiTest(UiBase* main, const char* name)
        : main_(main),
          state_(TS_INIT),
          name_(::strdup(name)),
          started_(false)
{
}

UiTest::~UiTest()
{
    if (started_) wait();
    ::free((void*)name_);
}

void UiTest::Draw()
{
}

void UiTest::OnKey(int value)
{
    gr_info("%s %d main: %p\n", __func__, value, main_);
    UiBase::SetCurrentUI(main_);
}

void UiTest::OnLeftTouch(int value)
{
    gr_info("%s %d\n", __func__, value);
    fail();
}

void UiTest::OnRightTouch(int value)
{
    gr_info("%s %d\n", __func__, value);
    pass();
}

void UiTest::OnAlarm()
{
    gr_info("alarm");
    UiBase::SetCurrentUI(main_);
}

void UiTest::OnEnter()
{
    gr_info("enter %p", this);
    set_alarm(5);
}

void UiTest::OnLeave()
{
    gr_info("leave %p", this);
}

void UiTest::RunTest()
{
    pass();
}

void* UiTest::do_test(void *thiz)
{
    UiTest* pThis = (UiTest*)thiz;
    pThis->RunTest();
    return NULL;
}

void UiTest::start()
{
    pthread_create(&worker_, NULL, do_test, this);
    started_ = true;
}


void UiTest::wait()
{
    pthread_join(worker_, NULL);
    started_ = false;
}
