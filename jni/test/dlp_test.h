//
// Created by dev on 16-12-9.
//

#ifndef RFTM_DLP_TEST_H
#define RFTM_DLP_TEST_H

#include "../ui_test.h"
#include "../blocking_queue.h"

class DlpTest : public UiUserJudgeTest
{
public:
    USER_JUDGE_TEST_ENTRY(DlpTest) {}

private:
    void Draw();

    void OnEnter();

    void OnRightTouch(int value);

    void RunTest();

    void scan_images();

    void next_image();

    bool has_next_image();

    std::vector<std::string> images_;
    int queued_image_index_;
    BlockingQueue<std::string> display_queue_;
};



#endif //RFTM_DLP_TEST_H
