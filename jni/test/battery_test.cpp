//
// Created by dev on 16-12-7.
//

#include "battery_test.h"

#define BATTERY_PRESENT "/sys/class/power_supply/battery/present"
#define CHARGER_ONLINE    "/sys/class/power_supply/ac/online"
#define VOLTAGE_PATH    "/sys/class/power_supply/battery/voltage_now"
#define CURRENT_PATH    "/sys/class/power_supply/battery/current_now"
#define CAPACITY_PATH   "/sys/class/power_supply/battery/capacity"

void BatteryTest::RunTest()
{
    clear_judge_result();
    wait_for_judge_result();
    clear_judge_result();
}

void BatteryTest::Draw()
{
    if (timer_.elapsed() - last_time_ > kSampleTime) {
        std::string out;
        bool charger_online = false;
        bool battery_present = false;
        int battery_voltage = 0;
        int battery_capacity = 0;
        int battery_current = 0;

#define CAT(path) \
        do { \
            if (run_command(format_string("cat %s", path), &out) < 0) { \
                XLOGE("read %s failed, %s", path, strerror(errno)); \
                return; \
            } \
        } while (0)

        CAT(BATTERY_PRESENT);
        battery_present = ::atoi(out.c_str()) > 0;

        CAT(CHARGER_ONLINE);
        charger_online = ::atoi(out.c_str()) > 0;

        CAT(CAPACITY_PATH);
        battery_capacity = ::atoi(out.c_str());

        CAT(VOLTAGE_PATH);
        battery_voltage = ::atoi(out.c_str());

        CAT(CURRENT_PATH);
        battery_current = ::atoi(out.c_str());

#undef CAT

        result(format_string("Charger: %s\n"
                                     "Battery: %s\n"
                                     "Voltage: %1.3fV \n"
//                                     "Current: %1.3fA \n"
                                     "Capacity: %d%%   ",
                             charger_online ? "online " : "offline",
                             battery_present ? "present" : "none   ",
                             battery_voltage / 1e6,
//                             battery_current / 1e6, // FIXME
                             battery_capacity));
        UiTest::Draw();
        gr_flip();

        last_time_ = timer_.elapsed();
    }
}
