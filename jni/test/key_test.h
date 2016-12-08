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

    bool check();

    static const int kValues = 8;
    std::string markL_;
    std::string markR_;
};


#endif //RFTM_TOUCH_TEST_H
