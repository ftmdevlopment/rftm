//
// Created by dev on 16-12-10.
//

#include "clear_flash.h"

void ClearFlash::RunTest()
{
    std::string out, userdata, cache;

    result("confirm/cancel ?");

    clear_judge_result();
    if (!wait_for_judge_result()) {
        XLOGI("action canceled!");
        result("");
        state(TS_TESTING);
        return;
    }

#define EXEC(cmd) \
    do { \
        run_command(cmd, &out); \
        if (out.find("not found") != out.npos) { \
            fail(); \
            clear_judge_result();\
            return; \
        } \
    } while (0)

    EXEC("mount | grep userdata | cut -d' ' -f1");
    userdata = trim_string(out);

    result("clear /data...");
    EXEC("make_ext4fs " + userdata);
    if (out.find("Created filesystem with") != out.npos) {
        append("clear /data done");
    }
    sleep(2);

    EXEC("mount | grep cache | cut -d' ' -f1");
    cache = trim_string(out);

    append("clear /cache...");
    EXEC("make_ext4fs " + cache);
    if (out.find("Created filesystem with") != out.npos) {
        append("clear /cache done");
    }
    sleep(2);

#undef EXEC

    pass();
    clear_judge_result();
}

void ClearFlash::OnRightTouch(int value)
{
    if (value != 0x8 && value != 0x10) {
        return;
    }
    UiUserJudgeTest::OnRightTouch(value);
}

void ClearFlash::OnLeftTouch(int value)
{
    if (value != 0x8 && value != 0x10) {
        return;
    }
    UiUserJudgeTest::OnLeftTouch(value);
}

void ClearFlash::append(std::string txt)
{
    result(result() + "\n" + txt);
}

