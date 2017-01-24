//
// Created by dev on 16-12-7.
//

#ifndef RFTM_TOUCH_TEST_H
#define RFTM_TOUCH_TEST_H

#include "../ui_test.h"

#include <stdatomic.h>  // in C11 standard

class KeyTest : public UiTest
{
public:
    UITEST_ENTRY(KeyTest) { }

private:
    void RunTest();

    void OnEnter();

    void OnLeave();

    void OnKey(int code, int value);

    void OnLeftTouch(int value);

    void OnRightTouch(int value);

    bool check();

    void update_and_check_result();

    int get_left_time();
    void set_left_time(int n);
    void sub_left_time(int n);

    int get_lock_free_type();

private:
    static const int kTestSeconds = 30;
    static const int kValues = 8;

    std::string left_;
    std::string right_;
    char top_;
    atomic_int time_left_;  // for some key FAILED
};


#endif //RFTM_TOUCH_TEST_H
