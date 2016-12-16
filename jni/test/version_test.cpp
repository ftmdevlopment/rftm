//
// Created by dev on 16-12-6.
//

#include <dirent.h>
#include "version_test.h"

static const std::string kXmosVID = "20b1";
static const std::string kXmosPID = "0008";

std::string get_xmos_version()
{
    std::string result;

    const std::string base = "/sys/bus/usb/devices/";
    DIR* dptr = opendir(base.c_str());
    dirent* de = NULL;
    while ((de = readdir(dptr)) != NULL) {
        if (de->d_name[0] == '.') continue;

        char link[PATH_MAX];
        readlink((base + de->d_name).c_str(), link, sizeof(link));

        std::string vidp = base + link + "/idVendor";
        std::string pidp = base + link + "/idProduct";

        if (file_exists(vidp.c_str()) && file_exists(pidp.c_str())) {
            std::string vid, pid;
            read_file(vidp.c_str(), &vid, 5); vid = trim_string(vid);
            read_file(pidp.c_str(), &pid, 5); pid = trim_string(pid);
            printf("found VID-PID: `%s-%s`\n", vid.c_str(), pid.c_str());
            if (vid == kXmosVID && pid == kXmosPID) {
                std::string bcd = base + link + "/bcdDevice";
                read_file(bcd.c_str(), &result, 16);
                result = trim_string(result);
                break;
            }
        }
    }
    closedir(dptr);
    return result;
}

void VersionTest::RunTest()
{
    std::string serial;
    run_command("getprop ro.serialno", &serial);
    serial = trim_string(serial);
    XLOGI("serialno: %s", serial.c_str());

    std::string out;
    run_command("cat /proc/version", &out);
    out = trim_string(out);

    std::string kernel = out.substr(sizeof("Linux version ")-1, sizeof("3.10.61")-1);
    std::string build = out.substr(out.length() - (sizeof("Nov 28 20:50:39 CST 2016")-1));
    build.resize(sizeof("Nov 28 20:50")-1);
    XLOGI("kernel: %s", kernel.c_str());
    XLOGI("build: %s", build.c_str());

    // for XMOS version
    std::string xmos = get_xmos_version();
    XLOGI("XMOS version: `%s`", xmos.c_str());

    result(format_string("SN: %s\n", serial.c_str())
           + format_string("Kernel: %s\n", kernel.c_str())
           + format_string("Build: %s\n", build.c_str())
           + format_string("XMOS: %s\n", xmos.c_str())
           + format_string("FTM: %s", FTM_VERSION));
    if (serial.empty() || kernel.empty() || xmos.empty()) {
        fail();
    } else {
        pass();
    }
    set_alarm_ms(1);
}
