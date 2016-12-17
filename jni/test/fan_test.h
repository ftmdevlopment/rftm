//
// Created by dev on 16-12-7.
//

#ifndef RFTM_FAN_TEST_H
#define RFTM_FAN_TEST_H

#include "../ui_test.h"
#include "../blocking_queue.h"
#include "../timer.h"

class FanTest : public UiUserJudgeTest
{
public:
    USER_JUDGE_TEST_ENTRY(FanTest) {  }

private:
    void RunTest();

    void Draw();

private:
    static constexpr double kTempSampleTime = 0.5;
    double last_time_;
    Timer timer_;
};


#endif //RFTM_FAN_TEST_H
