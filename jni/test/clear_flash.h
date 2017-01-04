//
// Created by dev on 16-12-10.
//

#ifndef RFTM_CLEAR_FLASH_H
#define RFTM_CLEAR_FLASH_H

#include "../ui_test.h"

class ClearFlash : public UiAutoJudgeTest
{
public:
    AUTO_JUDGE_TEST_ENTRY(ClearFlash) {}

private:
    void RunTest();
};


#endif //RFTM_CLEAR_FLASH_H
