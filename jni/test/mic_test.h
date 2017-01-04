//
// Created by dev on 16-12-13.
//

#ifndef RFTM_MIC_TEST_H
#define RFTM_MIC_TEST_H

#include "../ui_test.h"

class MicTest : public UiAutoJudgeTest
{
public:
    AUTO_JUDGE_TEST_ENTRY(MicTest) { }

private:
    void Draw();

    void RunTest();
};


#endif //RFTM_MIC_TEST_H
