//
// Created by dev on 16-12-15.
//

#ifndef RFTM_CAMERA_TEST_H
#define RFTM_CAMERA_TEST_H

#include "../ui_test.h"
#include "../blocking_queue.h"

class CameraTest : public UiTest
{
public:
    UITEST_ENTRY(CameraTest) {}

private:
    void Draw() {}

    void OnLeftTouch(int value);

    void OnRightTouch(int value);

    void RunTest();

private:
    BlockingQueue<bool> done_;
};


#endif //RFTM_CAMERA_TEST_H
