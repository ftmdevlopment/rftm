//
// Created by dev on 16-12-6.
//

#include "version_test.h"

void VersionTest::RunTest()
{
    std::string serial;
    run_command("getprop ro.serialno", &serial);
    serial = trim_string(serial);
    XLOGI("serialno: %s", serial.c_str());

    std::string out;
    run_command("cat /proc/version", &out);
    out = trim_string(out);
    std::string kversion = out.substr(sizeof("Linux version ")-1, sizeof("3.10.61")-1);
    std::string tbuild = out.substr(out.length() - (sizeof("Nov 28 20:50:39 CST 2016")-1));
    tbuild.replace(sizeof("Nov 28 20:50:39"), sizeof(" CST")-1, "");
    result(serial + '\n' + kversion + '\n' + tbuild);
    XLOGI("/proc/version: %s, kernel version: `%s`, build time: `%s`", out.c_str(), kversion.c_str(), tbuild.c_str());
}
