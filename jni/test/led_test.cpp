//
// Created by dev on 16-12-7.
//

#include <fcntl.h>

#include "led_test.h"

#define ALOGD XLOGI

#ifdef FTM_BIN_PATH
static const char* kXmosLedTestBin = FTM_BIN_PATH "ledtest";
static const char* kXmosLedOffBin = FTM_BIN_PATH "ledoff";
#else  // FTM_BIN_PATH
static const char* kXmosLedTestBin = "/system/bin/ledtest";
static const char* kXmosLedOffBin = "/system/bin/ledoff";
#endif // FTM_BIN_PATH

void LedTest::RunTest()
{
#if 0
    unsigned char white_color[42] = {
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    };
    unsigned char red_color[42] = {
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0x00, 0x00, 0xff, 0x00, 0x00, 0xff,
            0x00, 0x00, 0xff, 0x00, 0x00, 0xff,
            0x00, 0x00, 0xff, 0x00, 0x00, 0xff,
            0x00, 0x00, 0xff, 0x00, 0x00, 0xff,
    };
    unsigned char green_color[42] = {
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0x00, 0xff, 0x00, 0x00, 0xff, 0x00,
            0x00, 0xff, 0x00, 0x00, 0xff, 0x00,
            0x00, 0xff, 0x00, 0x00, 0xff, 0x00,
            0x00, 0xff, 0x00, 0x00, 0xff, 0x00,
    };
    unsigned char zero_buf[42] = {0};

    int fd, ret;

    fd = open("/dev/dm163", O_RDWR | O_NONBLOCK);
    if (fd < 0) {
        ALOGD("Failed to open /dev/dm163");
        goto FAIL;
    }

    ret = write(fd, red_color, sizeof(red_color));
    if (ret < 0) {
        ALOGD("Failed to write /dev/dm163");
        goto FAIL;
    }
    result("LED Red");
    sleep(2);

    ret = write(fd, green_color, sizeof(green_color));
    if (ret < 0) {
        ALOGD("Failed to write /dev/dm163");
        goto FAIL;
    }
    result("LED Green");
    sleep(2);

    ret = write(fd, white_color, sizeof(white_color));
    if (ret < 0) {
        ALOGD("Failed to write /dev/dm163");
        goto FAIL;
    }
    result("LED White");
    sleep(2);

    ret = write(fd, zero_buf, sizeof(zero_buf));
    if (ret < 0) {
        ALOGD("Failed to write /dev/dm163");
        goto FAIL;
    }
    result("turn off LED");
    sleep(2);
#endif

    std::string out;
#define EXEC(cmd) \
    do { \
        run_command(cmd, &out); \
        if (out.find("not found") != out.npos || out.find("No such file") != out.npos) { \
            goto FAIL; \
        } \
    } while (0)

    int brightness = 120;
    EXEC(format_string("%s %d", kXmosLedTestBin, brightness));

    clear_judge_result();
    result("PASSS/FAIL?");

    result(wait_for_judge_result() ? "Passed" : "Failed");

    XLOGI("try %s", kXmosLedOffBin);

    EXEC(kXmosLedOffBin);
    clear_judge_result();
    return ;

FAIL:
//    close(fd);
    fail();
    clear_judge_result();
}

