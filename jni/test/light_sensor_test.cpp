//
// Created by dev on 16-12-8.
//

#include "light_sensor_test.h"

#include <set>

void LightSensorTest::OnEnter()
{
    UiTest::OnEnter();
    write_file("/sys/class/input/input4/enable", "1");
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
        char text[64];
        snprintf(text, sizeof(text), "sensor value: %d", ev->value);
        result(text);

        XLOGI("sensor event %d", ev->value);
        if (values_.size() < kSampleTimes) {
            values_.put(ev->value);
        }
    }
}

void LightSensorTest::RunTest()
{
    std::set<int> uniq;
    for (int i = 0; i < kSampleTimes; i++) {
        uniq.insert(values_.take());
    }
    XLOGI("uniq: %d", uniq.size());
    if (uniq.size() > 1) {
        pass();
    } else {
        fail();
    }
}

