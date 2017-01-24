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

pthread_mutex_t g_ui_mutex;
static UiBase *pLeaveUi = NULL;
static UiBase *pEnterUi = NULL;
static UiBase *pCurrentUi = NULL;

void dlp_init();
struct GlobalConext
{
    GlobalConext()
    {
        dlp_init();
        gr_init();
        ev_init(UiBase::event_callback, NULL);
        pthread_mutex_init(&g_ui_mutex, NULL);
    }

    ~GlobalConext() {
        gr_exit();
        pthread_mutex_destroy(&g_ui_mutex);
    }
};

static GlobalConext __global_context__;

void UiBase::SetExpectedFPS(int fps)
{
    s_fps_expected = fps;
}

int UiBase::GetExpectedFPS()
{
    return s_fps_expected;
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
    ScopedLock _lock(&g_ui_mutex);
    pLeaveUi = pCurrentUi;
    pEnterUi = ui;
    pCurrentUi = ui;
}

UiBase* UiBase::GetCurrentUI()
{
    ScopedLock _lock(&g_ui_mutex);
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

void UiBase::OnKey(int code, int value)
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

void UiBase::OnEvent(int fd, struct input_event* ev, void* data)
{
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

    UiBase::Event event;
    if (ev.type == EV_KEY) {
        if (strstr(path, "event5")) {
            event.source = UiBase::UI_POWER_KEY;
        } else if (strstr(path, "event2")) {
            event.source = UiBase::UI_TOP_KEY;
            /**
             * workaround: on rokid device driver,
             *  both of the rear key and the top key
             *  were reported as KEY_POWER to userspace
             *  so we remark it at here.
             * */
            if (ev.code == KEY_POWER) {
                ev.code = KEY_OK;
            }
        } else {
            XLOGE("unkow key event...");
        }
    } else if (strstr(path, "event1")) {
        event.source = UiBase::UI_LEFT_TOUCH;
    } else if (strstr(path, "event0")) {
        event.source = UiBase::UI_RIGHT_TOUCH;
    } else {
        event.source = UiBase::UI_UNKNOW;
    }
    event.value = ev.value;
    event.fd = fd;
    event.ev = ev;
    event.data = data;

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

    while (s_running) {
        clock_gettime(CLOCK_REALTIME, &ts_start);

        UiBase *pCurrent = NULL, *pEnter = NULL, *pLeave = NULL;
        {
            ScopedLock _lock(&g_ui_mutex);
            pCurrent = pCurrentUi;
            pEnter = pEnterUi;
            pLeave = pLeaveUi;
        }

        // check and fire enter/leave callback
        if (pLeave) {
            {
                ScopedLock _lock(&g_ui_mutex);
                pLeaveUi = NULL;
            }
            pLeave->OnLeave(); // may call SetCurrentUI
        }
        if (pEnter) {
            {
                ScopedLock _lock(&g_ui_mutex);
                pEnterUi = NULL;
            }
            pEnter->last_alarm_ts_ = ts_start; // refresh alarm start
            pEnter->OnEnter();
        }

        // check and handle alarm
        long diff_ms = diff_timespec_us(&ts_start, &pCurrent->last_alarm_ts_)/US_PER_MS;
        if (pCurrent->alarm_ms_ > 0 && diff_ms >= pCurrent->alarm_ms_) {
            pCurrent->alarm_ms_ = 0; // means alarmed
//            XLOGI("alarm timestamp: %ld %ld", pCurrent->last_alarm_ts_.tv_sec, pCurrent->last_alarm_ts_.tv_nsec);
            pCurrent->OnAlarm(); // OnAlarm maybe recall set_alarm, so DO NOT MOVE IT UP.
        }

        // draw frame
        pCurrent->Draw();

        // merge and handle events
        Event event;
        bool ignore = false;
        bool merged = merge_events(&event, event_queue.peek_all());
        if (merged) {
            int type = event.ev.type;
            int value = event.ev.value;
            if (s_ignore_release && (type == EV_KEY || type == EV_ABS) && value == 0) {
                ignore = true;
            }
        }
        if (merged && !ignore) {
            pCurrent->OnEvent(event.fd, &event.ev, event.data);
            switch (event.source) {
                case UiBase::UI_POWER_KEY:
                case UiBase::UI_TOP_KEY:
                    pCurrent->OnKey(event.ev.code, event.value);
                    break;
                case UiBase::UI_LEFT_TOUCH:
                    pCurrent->OnLeftTouch(event.value);
                    break;
                case UiBase::UI_RIGHT_TOUCH:
                    pCurrent->OnRightTouch(event.value);
                    break;
                case UiBase::UI_UNKNOW:
                    break;
                default:
                    break;
            }
        }

        clock_gettime(CLOCK_REALTIME, &ts_end);

        long cost_us = diff_timespec_us(&ts_end, &ts_start);
        last_frame_cost_ = cost_us;
        long expected_cost_us = US_PER_S / s_fps_expected;
        long pad = expected_cost_us - cost_us;
        while (pad < 0) pad += expected_cost_us;
//        XLOGI("frame cost actual: %06d, expected %06d, pad: %06d", cost_us, expected_cost_us, pad);
        usleep(pad);
    }

    pthread_join(event_reader, NULL);
}

double UiBase::get_last_alarm_ts()
{
    return last_alarm_ts_.tv_sec + last_alarm_ts_.tv_nsec/1e9;
}

long UiBase::get_last_frame_cost()
{
    return last_frame_cost_;
}

