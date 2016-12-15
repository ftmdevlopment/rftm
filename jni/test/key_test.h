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

    void OnKey(int value);

    void OnLeftTouch(int value);

    void OnRightTouch(int value);

    void OnAlarm();

    bool check();

    void update_reult();

private:
    static const int kTestSeconds = 20;
    static const int kValues = 8;

    int time_left_;
    std::string markL_;
    std::string markR_;
};


#endif //RFTM_TOUCH_TEST_H
