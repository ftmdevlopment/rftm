//
// Created by dev on 16-12-7.
//

#ifndef RFTM_CHARGE_TEST_H
#define RFTM_CHARGE_TEST_H

#include "../ui_test.h"
#include "../blocking_queue.h"
#include "../timer.h"

class BatteryTest : public UiUserJudgeTest
{
public:
    USER_JUDGE_TEST_ENTRY(BatteryTest) {}

private:
    void RunTest();

    void Draw();

private:
    static constexpr double kSampleTime = 0.5;
    double last_time_;
    Timer timer_;
};


#endif //RFTM_CHARGE_TEST_H
