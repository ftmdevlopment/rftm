//
// Created by xu on 17-1-10.
//

#ifndef RFTM_DLP_AGE_TEST_H
#define RFTM_DLP_AGE_TEST_H

#include "../ui_test.h"

class DlpAgeTest : public UiUserJudgeTest
{
public:
    USER_JUDGE_TEST_ENTRY(DlpAgeTest) {}

private:
    void RunTest();

    void OnKey(int code, int value);

    BlockingQueue<bool> cancel_;
};


#endif //RFTM_DLP_AGE_TEST_H
