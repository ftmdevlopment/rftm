//
// Created by xu on 17-1-19.
//

#ifndef RFTM_DRAW_CIRCLE_TEST_H
#define RFTM_DRAW_CIRCLE_TEST_H

#include "../ui_test.h"

class DrawCircleTest : public UiAutoJudgeTest
{
public:
    AUTO_JUDGE_TEST_ENTRY(DrawCircleTest) {}

private:
    void Draw();

    void RunTest();
};


#endif //RFTM_DRAW_CIRCLE_TEST_H
