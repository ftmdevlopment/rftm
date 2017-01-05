//
// Created by dev on 16-12-10.
//

#ifndef RFTM_CLEAR_FLASH_H
#define RFTM_CLEAR_FLASH_H

#include "../ui_test.h"

class ClearFlash : public UiUserJudgeTest
{
public:
    USER_JUDGE_TEST_ENTRY(ClearFlash) {}

private:
    void OnLeftTouch(int value);

    void OnRightTouch(int value);

    void RunTest();
};


#endif //RFTM_CLEAR_FLASH_H
