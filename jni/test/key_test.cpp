//
// Created by dev on 16-12-7.
//

#include "key_test.h"

void KeyTest::OnEnter()
{
    UiTest::OnEnter();
    markL_ = std::string(kValues, ' ');
    markR_ = std::string(kValues, ' ');
    time_left_ = kTestSeconds;
    set_alarm(1);
}

void KeyTest::OnLeave()
{
    if (check()) {
        pass();
    } else {
        fail();
    }
    UiTest::OnLeave();
}

bool KeyTest::check()
{
    for (int i = 0; i < kValues; i++) {
        if (markL_[i] != '#' || markR_[i] != '#') {
            return false;
        }
    }
    return true;
}

void KeyTest::OnKey(int value)
{
    UiTest::OnKey(value);
}

void KeyTest::OnLeftTouch(int value)
{
    XLOGI("value: %x", value);
    int v[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};
    for (int i = 0; i < sizeof(v)/sizeof(v[0]); i++) {
        if (value == v[i]) {
            markL_[i] = '#';
        }
    }
    update_reult();
}

void KeyTest::OnRightTouch(int value)
{
    XLOGI("value: %x", value);
    int v[] = {0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1};
    for (int i = 0; i < sizeof(v)/sizeof(v[0]); i++) {
        if (value == v[i]) {
            markR_[i] = '#';
        }
    }
    update_reult();
}

void KeyTest::update_reult()
{
    result("L:" + markL_ + "\n"
           + "R:" + markR_ + "\n"
           + format_string("time left: %ds", time_left_));
}

void KeyTest::OnAlarm()
{
    if (time_left_ > 0) {
        time_left_--;
        set_alarm(1);
        update_reult();
    } else {
        UiTest::OnAlarm();
    }
}
