//
// Created by dev on 16-12-8.
//

#include "light_sensor_test.h"

#include <set>

void LightSensorTest::OnEnter()
{
    UiTest::OnEnter();
    write_file("/sys/class/input/input4/enable", "1");
    publish_sensor_value(0);
}

void LightSensorTest::OnLeave()
{
    write_file("/sys/class/input/input4/enable", "0");
    UiTest::OnLeave();
}

void LightSensorTest::OnEvent(int fd, struct input_event* ev, void* data)
{
    UiBase::OnEvent(fd, ev, data);

    if (ev->type == EV_ABS && ev->code == 0x28) {
        publish_sensor_value(ev->value);
        XLOGI("sensor event %d", ev->value);
    }
}

void LightSensorTest::RunTest()
{
    clear_judge_result();

    wait_for_judge_result();
}

void LightSensorTest::publish_sensor_value(int value)
{
    char text[64];
    snprintf(text, sizeof(text), "sensor value: %d", value);
    result(text);
}

