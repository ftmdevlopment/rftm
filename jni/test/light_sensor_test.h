//
// Created by dev on 16-12-8.
//

#ifndef RFTM_LIGHT_SENSOR_TEST_H
#define RFTM_LIGHT_SENSOR_TEST_H

#include "../ui_test.h"
#include "../blocking_queue.h"
#include <stdatomic.h>

class LightSensorTest : public UiAutoJudgeTest
{
public:
    AUTO_JUDGE_TEST_ENTRY(LightSensorTest) {}
private:
    void OnEnter();

    void OnLeave();

    void OnEvent(int fd, struct input_event* ev, void* data);

    void RunTest();

    void set_sensor_value(int value);
    int get_sensor_value();

private:
    const static int kTestSeconds = 10;
    atomic_int sensor_value_;
};


#endif //RFTM_LIGHT_SENSOR_TEST_H
