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

    void OnRightTouch(int value);

    void RunTest();

    void scan_images();

    void next_image();

    bool has_next_image();

    static const int kDiplayTime = 2;
    BlockingQueue<std::string> show_;
    std::vector<std::string> images_;
    int current_displayed_;
    pthread_mutex_t mutex_;
};



#endif //RFTM_DLP_TEST_H
