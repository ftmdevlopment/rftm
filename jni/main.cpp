//
// Created by dev on 16-12-3.
//

#include <fcntl.h>
#include <stdlib.h>
#include "ui_main.h"

int main(int argc, char** argv)
{
    UiBase::SetIgnoreRelease(true);

    UiBase* pUi = new UiMain();

    pUi->run();

    delete pUi;

    return 0;
}

static const char* DLP_MODE_PATH = "/sys/class/dlp/dlpc2607/dlp_mode";
static const char* DLP_ENABLE_PATH = "/sys/class/dlp/dlpc2607/enable";
static const int kPreferDlpMode = 4;

void dlp_init() {
    std::string value;
    char buffer[20];

    read_file(DLP_MODE_PATH, &value);
    if (::atoi(value.c_str()) != kPreferDlpMode) {
        snprintf(buffer, sizeof(buffer), "%d\n", kPreferDlpMode);
        write_file(DLP_MODE_PATH, buffer);
        printf("update DLP mode %d, %d\n", ::atoi(value.c_str()), kPreferDlpMode);
    }

    read_file(DLP_ENABLE_PATH, &value);
    if (::atoi(value.c_str()) != 1) {
        snprintf(buffer, sizeof(buffer), "%d\n", 1);
        write_file(DLP_ENABLE_PATH, buffer);
        printf("enable DLP\n");
    }
};
