//
// Created by dev on 16-12-6.
//

#ifndef RFTM_VERSION_TEST_H
#define RFTM_VERSION_TEST_H

#include "../ui_test.h"

class VersionTest : public UiAutoJudgeTest
{
public:
    AUTO_JUDGE_TEST_ENTRY(VersionTest) {}

private:
    void RunTest();
};


#endif //RFTM_VERSION_TEST_H
