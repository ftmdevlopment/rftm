//
// Created by dev on 16-12-10.
//

#ifndef RFTM_WIFI_TEST_H
#define RFTM_WIFI_TEST_H

#include "../ui_test.h"

class WifiTest : public UiAutoJudgeTest
{
public:
    AUTO_JUDGE_TEST_ENTRY(WifiTest) {}

private:
    void RunTest();
};

#endif //RFTM_WIFI_TEST_H
