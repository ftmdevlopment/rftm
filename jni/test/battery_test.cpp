//
// Created by dev on 16-12-7.
//

#include "battery_test.h"

#define BATTERY_PRESENT 	"/sys/class/power_supply/battery/present"
#define CHARGER_ONLINE 	"/sys/class/power_supply/ac/online"
#define VOLTAGE_PATH 	"/sys/class/power_supply/battery/voltage_now"
#define CAPACITY_PATH 	"/sys/class/power_supply/battery/capacity"

void BatteryTest::RunTest()
{
    std::string out;
    bool charger_online = false;
    bool battery_present = false;
    int battery_voltage = 0;
    int battery_capacity = 0;
    bool success = false;

#define READ(path) \
    if (read_file(path, &out, 1024) < 0) { \
        XLOGE("read %s failed, %s", path, strerror(errno)); \
        goto FAILURE; \
    }

    for (int i = 0; i < 40; i++) {
        READ(BATTERY_PRESENT);
        battery_present = ::atoi(out.c_str()) > 0;

        READ(CAPACITY_PATH);
        battery_capacity = ::atoi(out.c_str());

        READ(VOLTAGE_PATH);
        battery_voltage = ::atoi(out.c_str());

        READ(CHARGER_ONLINE);
        charger_online = ::atoi(out.c_str()) > 0;

        result(format_string("charger: %s\n"
                             "battery: %s\n"
                             "voltage: % 2d     \n"
                             "capacity:% 2d     ",
                             charger_online ?  "online " : "offline",
                             battery_present ? "present" : "none   ",
                             battery_voltage, battery_capacity));
        if (battery_present && battery_voltage > 0) {
            success = true;
        }
        usleep(500 * 1000);
    }

#undef READ

    if (success) {
        pass();
        set_alarm_ms(1);
    }

FAILURE:
    fail();
    set_alarm_ms(1);
}
