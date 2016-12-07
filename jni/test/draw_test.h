//
// Created by dev on 16-12-6.
//

#ifndef RFTM_DRAW_TEST_H
#define RFTM_DRAW_TEST_H

#include "../ui_test.h"

class DrawTest : public UiTest
{
public:
    DrawTest(UiBase* main, const char* name);

private:
    void Draw();
};


#endif //RFTM_DRAW_TEST_H
