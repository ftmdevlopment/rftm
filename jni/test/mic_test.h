//
// Created by dev on 16-12-13.
//

#ifndef RFTM_MIC_TEST_H
#define RFTM_MIC_TEST_H

#include "../ui_test.h"

class MicTest : public UiTest
{
public:
    UITEST_ENTRY(MicTest) { }

private:
    void Draw();

    void RunTest();
};


#endif //RFTM_MIC_TEST_H
