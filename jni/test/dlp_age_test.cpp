//
// Created by xu on 17-1-10.
//

#include "dlp_age_test.h"

static const char* DLP_ENABLE_PATH = "/sys/class/dlp/dlpc2607/enable";

static const int kSecondsPerMinute = 60;
static const int kSecondsPerHour = 3600;

void DlpAgeTest::RunTest()
{
    for (int i = 0; i < 50; i++) {
        write_file(DLP_ENABLE_PATH, "0");
        sleep(1);

        result(format_string("%d times", (i+1)));
        write_file(DLP_ENABLE_PATH, "1");
        sleep(1);
    }

    int timeout = 4 * kSecondsPerHour;
    while (timeout--) {
        sleep(1);
        int hours = timeout / kSecondsPerHour;
        int mins = timeout % kSecondsPerHour / kSecondsPerMinute;
        int secs = timeout % kSecondsPerMinute;
        result(format_string("time left %d:%02d:%02d", hours, mins, secs));
    }
}


