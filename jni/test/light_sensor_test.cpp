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
        set_sensor_value(ev->value);
        XLOGI("sensor event %d", ev->value);
    }
}

void LightSensorTest::RunTest()
{
    std::set<int> values;

    int seconds = kTestSeconds;
    while (seconds-- > 0) {
        sleep(1);

        int value = get_sensor_value();
        result(format_string("sensor value: %d\n", value)
               + (seconds > 0 ? format_string("left time: %ds", seconds) : std::string("")));
        values.insert(value);
    }
    if (values.size() == 1) {
        fail();
    } else {
        pass();
    }
}

void LightSensorTest::set_sensor_value(int value)
{
    atomic_store(&sensor_value_, value);
}

int LightSensorTest::get_sensor_value()
{
    return atomic_load(&sensor_value_);
}

