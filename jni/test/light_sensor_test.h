//
// Created by dev on 16-12-8.
//

#ifndef RFTM_LIGHT_SENSOR_TEST_H
#define RFTM_LIGHT_SENSOR_TEST_H

#include "../ui_test.h"
#include "../blocking_queue.h"

class LightSensorTest : public UiUserJudgeTest
{
public:
    USER_JUDGE_TEST_ENTRY(LightSensorTest) {}
private:
    void OnEnter();

    void OnLeave();

    void OnEvent(int fd, struct input_event* ev, void* data);

    void RunTest();

    void publish_sensor_value(int value);
};


#endif //RFTM_LIGHT_SENSOR_TEST_H
