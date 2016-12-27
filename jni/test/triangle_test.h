//
// Created by dev on 16-12-7.
//

#ifndef RFTM_TRIANGLE_TEST_H
#define RFTM_TRIANGLE_TEST_H

#include "../ui_test.h"

class TirangleTest : public UiTest
{
public:
    TirangleTest(UiBase* main, const char* name);

private:
    void Draw();

    void RunTest();

    int radius_;
    triangle_t triangle_;
};


#endif //RFTM_TRIANGLE_TEST_H
