//
// Created by dev on 16-12-7.
//

#ifndef RFTM_TRIANGLE_TEST_H
#define RFTM_TRIANGLE_TEST_H

#include "../ui_test.h"

class TriangleTest : public UiTest
{
public:
    TriangleTest(UiBase* main, const char* name);

private:
    void OnEnter();

    void OnLeave();

    void Draw();

    void RunTest();

    int origin_fps_;

    int radius_;

    triangle_t T0;
    static const int N = 12;
    triangle_t triangles_[N];
    static const int edge_length_ = 20;
};

#endif //RFTM_TRIANGLE_TEST_H
