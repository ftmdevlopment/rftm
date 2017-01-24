//
// Created by dev on 16-12-7.
//

#include "key_test.h"

static const char kNoMark = '=';
static const char kMarked = '@';

void KeyTest::OnEnter()
{
    UiTest::OnEnter();

    XLOGI("atomic_int is lock-free: %d", get_lock_free_type());

    top_ = kNoMark;
    left_ = std::string(kValues, kNoMark);
    right_ = std::string(kValues, kNoMark);
    set_left_time(kTestSeconds);

    update_and_check_result();
}

void KeyTest::OnLeave()
{
    UiTest::OnLeave();
}

bool KeyTest::check()
{
    for (int i = 0; i < kValues; i++) {
        if (left_[i] == kNoMark || right_[i] == kNoMark) {
            return false;
        }
    }
    return top_ != kNoMark;
}

void KeyTest::OnKey(int code, int value)
{
    XLOGI("%s %d %d\n", __func__, code, value);
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
    int left_seconds = get_left_time();
    if (check()) {
        if (left_seconds > 0) {
            set_left_time(0);
        }
    }
    result("L:" + left_ + "\n"
           + "R:" + right_ + "\n"
           + "Top:" + top_ + "\n"
           + (left_seconds > 0 ? format_string("time left: %ds", left_seconds) : std::string()));
}

void KeyTest::RunTest()
{
    while (get_left_time() > 0) {
        update_and_check_result();
        sleep(1);
        sub_left_time(1);
    }
    if (check()) {
        pass();
    } else {
        fail();
    }
    sleep(1);
}

void KeyTest::set_left_time(int n)
{
    atomic_store(&time_left_, n);
}

int KeyTest::get_left_time()
{
    return atomic_load(&time_left_);
}

void KeyTest::sub_left_time(int n)
{
//  set_left_time(get_left_time() - n);
    atomic_fetch_sub(&time_left_, n);
}

int KeyTest::get_lock_free_type()
{
    return atomic_is_lock_free(&time_left_);
}
