//
// Created by dev on 16-12-15.
//

#ifndef RFTM_CAMERA_TEST_H
#define RFTM_CAMERA_TEST_H

#include "../ui_test.h"
#include "../blocking_queue.h"

class CameraTest : public UiUserJudgeTest
{
public:
    USER_JUDGE_TEST_ENTRY(CameraTest) {}

private:
    void Draw() {}

    void RunTest();
};


#endif //RFTM_CAMERA_TEST_H
