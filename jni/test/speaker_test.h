//
// Created by dev on 16-12-9.
//

#ifndef RFTM_SPEAKER_TEST_H
#define RFTM_SPEAKER_TEST_H

#include "../ui_test.h"
#include "../blocking_queue.h"

class SpeakerTest : public UiUserJudgeTest
{
public:
    USER_JUDGE_TEST_ENTRY(SpeakerTest) {}

private:
    void RunTest();
};


#endif //RFTM_SPEAKER_TEST_H
