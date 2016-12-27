//
// Created by dev on 16-12-7.
//

#include "fan_test.h"

void FanTest::RunTest()
{
    sleep(1);

    result("turn down fan...");
    write_file("/sys/class/fan/fan_duty/duty", "0");

    timer_.reset();
    last_time_ = timer_.elapsed() + 5.0;
    sleep(5);

    result("turn on fan...");
    write_file("/sys/class/fan/fan_duty/duty", "100");

    clear_judge_result();
    wait_for_judge_result();
    result("");
    clear_judge_result();
}

void FanTest::Draw()
{
    std::string temp;
    if (timer_.elapsed() - last_time_ > kTempSampleTime
        && read_file("/sys/class/thermal/thermal_zone1/temp", &temp) > 0) {
        int cpu_temp = ::atoi(temp.c_str());
        result(format_string("CPU Temp: %d.%d ^C\nplease check fan", cpu_temp/1000, cpu_temp%1000));
        last_time_ = timer_.elapsed();
    }
    UiTest::Draw();
}
