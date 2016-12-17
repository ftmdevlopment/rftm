//
// Created by dev on 16-12-3.
//

#include <fcntl.h>
#include "ui_main.h"

int main(int argc, char** argv)
{
    UiBase::SetIgnoreRelease(true);

    UiBase* pUi = new UiMain();

    pUi->run();

    delete pUi;

    return 0;
}

void dlp_init() {
    int fd_dlp_mod = open("/sys/class/dlp/dlpc2607/dlp_mode", O_RDWR);
    if (fd_dlp_mod > 0) {
        char buf[20];
        int b = sprintf (buf, "%d\n", 4);
        write(fd_dlp_mod, "4", b);
        close(fd_dlp_mod);
    } else {
        printf("can not write: /sys/class/dlp/dlpc2607/dlp_mode\n");
    }

    int fd_dlp_enable = open("/sys/class/dlp/dlpc2607/enable", O_RDWR);
    if (fd_dlp_enable > 0) {
        char buf[20];
        int b = sprintf(buf, "%d\n", 1);
        write(fd_dlp_enable, buf, b);
        close(fd_dlp_enable);
    }
    else {
        printf("can not write: /sys/class/dlp/dlpc2607/enable\n");
    }
};
