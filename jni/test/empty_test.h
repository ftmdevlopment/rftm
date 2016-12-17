//
// Created by dev on 16-12-14.
//

#ifndef RFTM_EMPTY_TEST_H
#define RFTM_EMPTY_TEST_H

#include "../ui_test.h"

class EmptyTest : public UiTest
{
public:
    UITEST_ENTRY(EmptyTest) {}

private:

    void Draw() { UiTest::Draw(); }

    void OnEnter() {}

    void OnLeave() {}

    void OnKey(int code, int value) { back(); }

    void OnLeftTouch(int value) { back(); }

    void OnRightTouch(int value) { back(); }
};


#endif //RFTM_EMPTY_TEST_H
