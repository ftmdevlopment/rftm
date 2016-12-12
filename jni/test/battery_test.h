//
// Created by dev on 16-12-7.
//

#ifndef RFTM_CHARGE_TEST_H
#define RFTM_CHARGE_TEST_H

#include "../ui_test.h"

class BatteryTest : public UiTest
{
public:
    UITEST_ENTRY(BatteryTest) {}

private:
    void RunTest();
};


#endif //RFTM_CHARGE_TEST_H
