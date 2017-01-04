//
// Created by dev on 16-12-10.
//

#ifndef RFTM_POWER_OFF_H
#define RFTM_POWER_OFF_H

#include "../ui_test.h"

class PowerOff : public UiAutoJudgeTest
{
public:
    AUTO_JUDGE_TEST_ENTRY(PowerOff) {}

private:
    void RunTest();
};


#endif //RFTM_POWER_OFF_H
