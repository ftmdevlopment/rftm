//
// Created by dev on 16-12-9.
//

#ifndef RFTM_DLP_TEST_H
#define RFTM_DLP_TEST_H

#include "../ui_test.h"
#include "../blocking_queue.h"

class DlpTest : public UiTest
{
public:
    UITEST_ENTRY(DlpTest) { done_ = false; }

private:
    void Draw();

    void RunTest();

    bool done_;
    BlockingQueue<std::string> images_;
};


#endif //RFTM_DLP_TEST_H
