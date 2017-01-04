//
// Created by dev on 16-12-10.
//

#ifndef RFTM_MEM_TEST_H
#define RFTM_MEM_TEST_H

#include "../ui_test.h"

class MemTest : public UiAutoJudgeTest
{
public:
    AUTO_JUDGE_TEST_ENTRY(MemTest) {}

private:
    void RunTest();

    void OnKey(int c, int v) {}

    void OnLeftTouch(int v) {}

    void OnRightTouch(int v) {}
};


#endif //RFTM_MEM_TEST_H
