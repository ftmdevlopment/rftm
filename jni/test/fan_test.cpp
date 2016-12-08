//
// Created by dev on 16-12-7.
//

#include "fan_test.h"

void FanTest::RunTest()
{
    int cpu_temp = 0, cpu_temp2 = 0;
    char buffer[64];
    std::string str_temp;

    if (read_file("/sys/class/thermal/thermal_zone1/temp", &str_temp) > 0) {
        sscanf(str_temp.c_str(), "%d", &cpu_temp);
        sprintf(buffer, "CPU Temp: %d.%d ^C", cpu_temp/1000, cpu_temp%1000);
        result(buffer);
        XLOGI("temp: %s, %s\n", str_temp.c_str(), buffer);
    }
    sleep(1);

    result("turn on fan...\n5s");
    write_file("/sys/class/fan/fan_duty/duty", "100");

    sleep(5);
    if (read_file("/sys/class/thermal/thermal_zone1/temp", &str_temp) > 0) {
        sscanf(str_temp.c_str(), "%d", &cpu_temp2);
        sprintf(buffer, "CPU Temp: %d.%d ^C", cpu_temp2/1000, cpu_temp2%1000);
        result(buffer);
        XLOGI("%s\n", buffer);
    }
    sleep(1);

    if (cpu_temp2 < cpu_temp) pass();

    write_file("/sys/class/fan/fan_duty/duty", "0");
}
