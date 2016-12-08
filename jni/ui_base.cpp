//
// Created by dev on 16-12-3.
//

#include <unistd.h>
#include <linux/input.h>
#include <linux/limits.h>

#include <time.h>
#include <stdio.h>
#include <stdint.h>

#include <map>

#include "log.h"
#include "ui_base.h"
#include "minui/minui.h"
#include "blocking_queue.h"

static const int MS_PER_S = 1000;
static const int US_PER_MS = 1000;
static const int NS_PER_US = 1000;

static const int US_PER_S = US_PER_MS * MS_PER_S;
static const int NS_PER_MS = NS_PER_US * US_PER_MS;
static const int NS_PER_S = NS_PER_US * US_PER_MS * MS_PER_S;

static pthread_t event_reader;
static BlockingQueue<UiBase::Event> event_queue(1024);

int UiBase::s_fps_expected = 50;  // default FPS
bool UiBase::s_running = true;
bool UiBase::s_ignore_release = false;

static int iInstanceCount = 0;
static UiBase *pLeaveUi = NULL;
static UiBase *pEnterUi = NULL;
static UiBase *pCurrentUi = NULL;

struct GlobalConext
{
    GlobalConext()
    {
        gr_init();
        ev_init(UiBase::event_callback, NULL);
    }

    ~GlobalConext() {
        gr_exit();
    }
};

static GlobalConext __global_context__;

void UiBase::SetExpectedFPS(int fps)
{
    s_fps_expected = fps;
}

void UiBase::SetRunning(bool running)
{
    s_running = running;
}

void UiBase::SetIgnoreRelease(bool ignore)
{
    s_ignore_release = ignore;
}

void UiBase::SetCurrentUI(UiBase *ui)
{
    pLeaveUi = pCurrentUi;
    pEnterUi = ui;
    pCurrentUi = ui;
}

UiBase* UiBase::GetCurrentUI()
{
    return pCurrentUi;
}


UiBase::UiBase()
{
    ++iInstanceCount;
}

UiBase::~UiBase()
{
    --iInstanceCount;
}

void UiBase::OnKey(int value)
{
    XLOGI("value: %d last_frame_cost_: % 6d", value, last_frame_cost_);
}

void UiBase::OnLeftTouch(int value)
{
    XLOGI("value: %d", value);
}

void UiBase::OnRightTouch(int value)
{
    XLOGI("value: %d", value);
}

void UiBase::OnAlarm()
{
    XLOGI("alarm ms: %d, ts: %ld, %ld", alarm_ms_, last_alarm_ts_.tv_sec, last_alarm_ts_.tv_nsec);
}

void UiBase::OnEnter()
{
    XLOGI("enter %p", this);
}

void UiBase::OnLeave()
{
    XLOGI("leave %p", this);
}

int UiBase::event_callback(int fd, uint32_t epevents, void *data)
{
    struct input_event ev;
    int ret = ev_get_input(fd, epevents, &ev);
    if (ret) return -1;

    char proc[PATH_MAX], path[PATH_MAX];
    snprintf(proc, sizeof(proc), "/proc/self/fd/%d", fd);
    readlink(proc, path, sizeof(path));

//    XLOGI("type: %x, code: %x, value: %x @ path: %s", ev.type, ev.code, ev.value, path);

    if (ev.type == EV_SYN) return 0;
    if (s_ignore_release && ev.value == 0) return 0;

    UiBase::Event event;
    if (ev.type == EV_KEY) {
        event.source = UiBase::UI_POWER_KEY;
    } else if (strstr(path, "event1")) {
        event.source = UiBase::UI_LEFT_TOUCH;
    } else if (strstr(path, "event0")) {
        event.source = UiBase::UI_RIGHT_TOUCH;
    } else {
        event.source = UiBase::UI_UNKNOW;
    }
    event.value = ev.value;
    event_queue.put(event);

    return 0;
}

static void* read_event(void *)
{
    for (;;) {
        if (!ev_wait(-1)) {
            ev_dispatch();
        }
    }
    return NULL;
}

static bool merge_events(UiBase::Event* result, const std::vector<UiBase::Event>& events)
{
    if (events.empty()) return false;

#if 0
    std::map<uint64_t, int> dict;
    for (size_t i = 0; i < events.size(); i++) {
        UiBase::Event e = events[i];
        uint64_t evt = e.source;
        evt <<= 32u;
        evt |=  e.value;
        dict[evt]++;
    }

    int m = 0;
    uint64_t e;
    for (auto& kv: dict) {
//        printf("k: %lx, v: %d\n", kv.first, kv.second);
        if (kv.second > m) {
            m = kv.second;
            e = kv.first;
        }
    }
//    printf("we pick %lx %d\n", e, m);

    result->source = (e >> 32ul);
    result->value = e & 0xfffffffful;
#else
    *result = events[0];
#endif
    return true;
}

static inline long diff_timespec_us(const struct timespec* end, const struct timespec* start)
{
    if (!start || !end) return 0;
    return (end->tv_sec - start->tv_sec) * US_PER_S + (end->tv_nsec - start->tv_nsec) / NS_PER_US;
}

void UiBase::set_alarm(long s)
{
    return set_alarm_ms(s*1000);
}

void UiBase::set_alarm_ms(long ms)
{
    alarm_ms_ = ms;
    clock_gettime(CLOCK_REALTIME, &last_alarm_ts_);
}

void UiBase::run()
{
    pthread_create(&event_reader, NULL, read_event, NULL);

    UiBase::SetCurrentUI(this);
    // main thread draw frames, merge and handle events etc ...
    struct timespec ts_start, ts_end;
    long expected_cost_us = US_PER_S / s_fps_expected;

    while (s_running) {
        clock_gettime(CLOCK_REALTIME, &ts_start);

        // check and fire enter/leave callback
        if (pLeaveUi) {
            UiBase* p = pLeaveUi;
            pLeaveUi = NULL;
            p->OnLeave(); // may call SetCurrentUI
        }
        if (pEnterUi) {
            UiBase* p = pEnterUi;
            pEnterUi = NULL;
            p->last_alarm_ts_ = ts_start; // refresh alarm start
            p->OnEnter();
        }

        // check and handle alarm
        long diff_ms = diff_timespec_us(&ts_start, &pCurrentUi->last_alarm_ts_)/US_PER_MS;
        if (pCurrentUi->alarm_ms_ > 0 && diff_ms >= pCurrentUi->alarm_ms_) {
            pCurrentUi->alarm_ms_ = 0; // means alarmed
//            XLOGI("alarm timestamp: %ld %ld", pCurrentUi->last_alarm_ts_.tv_sec, pCurrentUi->last_alarm_ts_.tv_nsec);
            pCurrentUi->OnAlarm(); // OnAlarm maybe recall set_alarm, so DO NOT MOVE IT UP.
        }

        // draw frame
        pCurrentUi->Draw();
        gr_flip();

        // merge and handle events
        Event event;
        if (merge_events(&event, event_queue.peek_all())) {
            switch (event.source) {
                case UiBase::UI_POWER_KEY:
                    pCurrentUi->OnKey(event.value);
                    break;
                case UiBase::UI_LEFT_TOUCH:
                    pCurrentUi->OnLeftTouch(event.value);
                    break;
                case UiBase::UI_RIGHT_TOUCH:
                    pCurrentUi->OnRightTouch(event.value);
                    break;
                default:
                    break;
            }
        }

        clock_gettime(CLOCK_REALTIME, &ts_end);

        long cost_us = diff_timespec_us(&ts_end, &ts_start);
        last_frame_cost_ = cost_us;
        long pad = expected_cost_us - cost_us;
        while (pad < 0) pad += expected_cost_us;
//        XLOGI("frame cost actual: %06d, expected %06d, pad: %06d", cost_us, expected_cost_us, pad);
        usleep(pad);
    }

    pthread_join(event_reader, NULL);
}

