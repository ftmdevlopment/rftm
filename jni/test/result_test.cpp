//
// Created by dev on 16-12-15.
//

#include "result_test.h"

#define msleep(ms) usleep((ms) * 1000)

void ResultTest::RunTest()
{
    set_alarm(0);

    std::string text;
    for (int i = 0; i < 5; i++) {
        text += format_string("%d\n", i);
        result(text);
        msleep(200);
    }

    text.clear();
    for (int i = 0; i < 100; i++) {
        text += "0123456789"[i % 10];
        result(text);
        msleep(50);
    }
    pass();

    set_alarm(1);
}
