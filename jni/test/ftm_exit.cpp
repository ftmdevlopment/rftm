//
// Created by dev on 16-12-10.
//

#include "ftm_exit.h"

#define FW_GETENV "/system/xbin/fw_printenv"
#define FW_SETENV "/system/xbin/fw_setenv"

void FtmExit::RunTest()
{
    std::string out, cur;
    run_command(FW_GETENV " ftm_mode", &out);
    if (out.find("not found") != out.npos) {
        XLOGE(FW_GETENV" not found");
        result(out);
        goto FAILURE;
    }
    cur = out;

    run_command(FW_SETENV " ftm_mode 0", &out);
    if (out.find("not found") != out.npos) {
        XLOGE(FW_SETENV" not found");
        result(out);
        goto FAILURE;
    }

    if (out != cur) {
        run_command(FW_GETENV " ftm_mode", &out);
        pass();
        return;
    }

FAILURE:
    fail();
}
