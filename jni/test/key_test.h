//
// Created by dev on 16-12-7.
//

#ifndef RFTM_TOUCH_TEST_H
#define RFTM_TOUCH_TEST_H

#include "../ui_test.h"

class KeyTest : public UiTest
{
public:
    UITEST_ENTRY(KeyTest) { }

private:
    void OnEnter();

    void OnLeave();

    void OnKey(int code, int value);

    void OnLeftTouch(int value);

    void OnRightTouch(int value);

    void OnAlarm();

    bool check();

    void update_and_check_result();

private:
    static const int kTestSeconds = 30;
    static const int kValues = 8;

    std::string left_;
    std::string right_;
    char top_;
    char rear_;
    int time_left_;  // for some key FAILED
};


#endif //RFTM_TOUCH_TEST_H
