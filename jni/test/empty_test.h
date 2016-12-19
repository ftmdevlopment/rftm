//
// Created by dev on 16-12-14.
//

#ifndef RFTM_EMPTY_TEST_H
#define RFTM_EMPTY_TEST_H

#include "../ui_test.h"

class EmptyTest : public UiTest
{
public:
    UITEST_ENTRY(EmptyTest) {}

private:
    void OnAlarm() { back(); }

    void OnEnter() { set_alarm_ms(1); }

    void OnLeave() {}

    void OnKey(int code, int value) {}

    void OnLeftTouch(int value) {}

    void OnRightTouch(int value) {}
};


#endif //RFTM_EMPTY_TEST_H
