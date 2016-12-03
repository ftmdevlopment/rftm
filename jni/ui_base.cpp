//
// Created by dev on 16-12-3.
//

#include <unistd.h>
#include <linux/input.h>
#include <linux/limits.h>

#include <stdio.h>

#include "ui_base.h"
#include "minui/minui.h"

static UiBase *pCurrentUi = NULL;
static int iInstanceCount = 0;

pthread_t UiBase::event_reader;

UiBase::UiBase()
{
    this->last_ui_ = pCurrentUi;
    pCurrentUi = this;

    ++iInstanceCount;
}

UiBase::~UiBase()
{
    --iInstanceCount;
}

int UiBase::event_callback(int fd, uint32_t epevents, void *data)
{
    struct input_event ev;
    int ret = ev_get_input(fd, epevents, &ev);
    if (ret) return -1;

    char proc[PATH_MAX], path[PATH_MAX];
    snprintf(proc, sizeof(proc), "/proc/self/fd/%d", fd);
    readlink(proc, path, sizeof(path));

    gr_info("type: %x, code: %x, value: %x @ path: %s", ev.type, ev.code, ev.value, path);

    if (ev.type == EV_SYN) return 0;
#if 0
    if (ev.type == EV_KEY) {

    } else if (strstr(path, "event1")) {
        printf("left touch %d %d\n", ev.code, ev.value);
        pCurrentUi->OnLeftTouch(ev.code, ev.value);
    } else if (strstr(path, "event0")) {
        printf("right touch %d %d\n", ev.code, ev.value);
        pCurrentUi->OnRightTouch(ev.code, ev.value);
    }

    pCurrentUi->Draw();
#endif
    return 0;
}

void* UiBase::read_event(void *)
{
    for (;;) {
        if (!ev_wait(-1)) {
            ev_dispatch();
        }
    }
    return NULL;
}

void UiBase::run()
{
    gr_init();
    ev_init(UiBase::event_callback, NULL);

    pCurrentUi->Draw();

    pthread_create(&event_reader, NULL, read_event, NULL);


    // main thread draw frames, merge input_event etc....


    pthread_join(event_reader, NULL);
    gr_exit();
}

