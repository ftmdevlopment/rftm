//
// Created by dev on 16-12-10.
//

#include "ftm_exit.h"

void FtmExit::RunTest()
{
    std::string out, cur;
    run_command("fw_getenv ftm_mode", &out);
    if (out.find("not found")) {
        XLOGE("fw_getenv not found");
        result("fw_getenv not found");
        goto FAILURE;
    }
    cur = out;

    run_command("fw_setenv ftm_mode 0", &out);
    if (out.find("not found")) {
        XLOGE("fw_setenv not found");
        result("fw_setenv not found");
        goto FAILURE;
    }

    if (out != cur) {
        pass();
        set_alarm_ms(1);
        return;
    }

FAILURE:
    fail();
    set_alarm_ms(1);
}
