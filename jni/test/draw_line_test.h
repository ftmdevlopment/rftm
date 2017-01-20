//
// Created by xu on 17-1-20.
//

#ifndef RFTM_DRAW_LINE_TEST_H
#define RFTM_DRAW_LINE_TEST_H

#include "../ui_test.h"

class DrawLineTest : public UiAutoJudgeTest
{
public:
    AUTO_JUDGE_TEST_ENTRY(DrawLineTest) {}

private:
    void OnEnter();

    void Draw();

    void RunTest();

private:
    static const int N = 12;
    std::pair<point_t, point_t> endpoints[N];
};


#endif //RFTM_DRAW_LINE_TEST_H
