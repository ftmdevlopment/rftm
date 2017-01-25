//
// Created by dev on 16-12-7.
//

#include "battery_test.h"

#include <stdlib.h>

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

static std::string cat(std::string path)
{
    std::string result;
    if (run_command("cat " + path, &result) < 0) {
        XLOGE("cat %s failed, %s", path.c_str(), strerror(errno));
    }
    return result;
}

void BatteryTest::Draw()
{
    if (timer_.elapsed() - last_time_ > kSampleTime) {
        bool charger_online = ::atoi(cat(CHARGER_ONLINE).c_str()) > 0;
        bool battery_present = ::atoi(cat(BATTERY_PRESENT).c_str()) > 0;

        int battery_voltage = ::atoi(cat(VOLTAGE_PATH).c_str());
        int battery_current = ::atoi(cat(CURRENT_PATH).c_str());

        int battery_capacity = ::atoi(cat(CAPACITY_PATH).c_str());

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
