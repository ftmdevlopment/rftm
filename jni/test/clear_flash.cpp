//
// Created by dev on 16-12-10.
//

#include "clear_flash.h"

void ClearFlash::RunTest()
{
    std::string out, userdata, cache;

#define EXEC(cmd) \
    do { \
        run_command(cmd, &out); \
        if (out.find("not found") != out.npos) { \
            goto FAILURE; \
        } \
    } while (0)

    EXEC("mount | grep userdata | cut -d' ' -f1");
    userdata = trim_string(out);

    result("clear /data...");
    EXEC("make_ext4fs " + userdata);
    if (out.find("Created filesystem with") != out.npos) {
        result("clear /data done");
    }
    sleep(2);

    EXEC("mount | grep cache | cut -d' ' -f1");
    cache = trim_string(out);

    result("clear /cache...");
    EXEC("make_ext4fs " + cache);
    if (out.find("Created filesystem with") != out.npos) {
        result("clear /cache done");
    }
    sleep(2);

#undef EXEC

    pass();
    set_alarm_ms(1);
    return;

FAILURE:
    fail();
    set_alarm_ms(1);
}

