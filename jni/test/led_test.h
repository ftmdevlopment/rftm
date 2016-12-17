//
// Created by dev on 16-12-7.
//

#ifndef RFTM_LED_TEST_H
#define RFTM_LED_TEST_H

#include "../ui_test.h"
#include "../blocking_queue.h"

class LedTest : public UiUserJudgeTest
{
public:
    USER_JUDGE_TEST_ENTRY(LedTest) {}

private:
    void RunTest();
};


#endif //RFTM_LED_TEST_H
