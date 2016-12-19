//
// Created by dev on 16-12-7.
//

#include "key_test.h"

static const char kNoMark = '=';
static const char kMarked = '@';

void KeyTest::OnEnter()
{
    UiTest::OnEnter();
    top_ = kNoMark;
    rear_ = kNoMark;
    left_ = std::string(kValues, kNoMark);
    right_ = std::string(kValues, kNoMark);
    time_left_ = kTestSeconds;
    update_and_check_result();
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
        if (left_[i] == kNoMark || right_[i] == kNoMark) {
            return false;
        }
    }
    return top_ != kNoMark && rear_ != kNoMark;
}

void KeyTest::OnKey(int code, int value)
{
    XLOGI("%s %d %d\n", __func__, code, value);
    if (code == KEY_POWER) {
        rear_ = kMarked;
    }
    if (code == KEY_OK) {
        top_ = kMarked;
    }
    update_and_check_result();
}

void KeyTest::OnLeftTouch(int value)
{
    XLOGI("value: %x", value);
    int v[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};
    for (int i = 0; i < sizeof(v)/sizeof(v[0]); i++) {
        if (value == v[i]) {
            left_[i] = kMarked;
        }
    }
    update_and_check_result();
}

void KeyTest::OnRightTouch(int value)
{
    XLOGI("value: %x", value);
    int v[] = {0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1};
    for (int i = 0; i < sizeof(v)/sizeof(v[0]); i++) {
        if (value == v[i]) {
            right_[i] = kMarked;
        }
    }
    update_and_check_result();
}

void KeyTest::update_and_check_result()
{
    result("L:" + left_ + "\n"
           + "R:" + right_ + "\n"
           + "Top:" + top_ + "\n"
           + "Power:" + rear_ + "\n"
           + format_string("time left: %ds", time_left_));
    if (check()) {
        if (time_left_ > 0) {
            result("L:" + left_ + "\n"
                   + "R:" + right_ + "\n"
                   + "Top:" + top_ + "\n"
                   + "Power:" + rear_ + "\n");
            time_left_ = 0;
        }
        pass();
    }
}

void KeyTest::RunTest()
{
    while (time_left_ > 0) {
        update_and_check_result();
        sleep(1);
        time_left_--;
    }
    fail();
}
