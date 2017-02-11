//
// Created by xu on 17-2-11.
//

#include <unistd.h>

#include <stdio.h>
#include <assert.h>

#include "minui/minui.h"
#include "ui_core.h"
#include "ui_base.h"
#include "log.h"


static const int MS_PER_S = 1000;
static const int US_PER_MS = 1000;
static const int NS_PER_US = 1000;

static const int US_PER_S = US_PER_MS * MS_PER_S;
static const int NS_PER_MS = NS_PER_US * US_PER_MS;
static const int NS_PER_S = NS_PER_US * US_PER_MS * MS_PER_S;


void dlp_init();

UiCore::UiCore()
{
    pthread_mutex_init(&mutex_, NULL);

    dlp_init();
    gr_init();
    ev_init(UiCore::handle_event, NULL);
}

UiCore::~UiCore()
{
    gr_exit();

    pthread_mutex_destroy(&mutex_);
}

UiCore& UiCore::instance()
{
    static UiCore stub;
    return stub;
}

// ensure gr_init before all other gr_* calls
static int __ui_core_init_stub__ = [](){
    UiCore::instance();
    return 0x900d;
}();

std::vector<UiCore::Event> UiCore::get_pending_events()
{
    return event_queue_.peek_all();
}

void UiCore::put_pending_event(Event event)
{
    event_queue_.put(event);
}

void UiCore::start_event_thread()
{
    set_running(true);
    pthread_create(&event_thread_, NULL, UiCore::read_event, NULL);
}

void UiCore::join_event_thread()
{
    pthread_join(event_thread_, NULL);
}

void UiCore::stop_event_thread()
{
    set_running(false);
}

int UiCore::handle_event(int fd, uint32_t epevents, void *data)
{
    struct input_event ev;
    int ret = ev_get_input(fd, epevents, &ev);
    if (ret) return -1;

    char proc[PATH_MAX], path[PATH_MAX];
    snprintf(proc, sizeof(proc), "/proc/self/fd/%d", fd);
    readlink(proc, path, sizeof(path));

//    XLOGI("type: %x, code: %x, value: %x @ path: %s", ev.type, ev.code, ev.value, path);

    Event event;
    if (ev.type == EV_KEY) {
        if (strstr(path, "event5")) {
            event.source = UI_POWER_KEY;
        } else if (strstr(path, "event2")) {
            event.source = UI_TOP_KEY;
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
            XLOGE("unknow key event...");
        }
    } else if (strstr(path, "event1")) {
        event.source = UI_LEFT_TOUCH;
    } else if (strstr(path, "event0")) {
        event.source = UI_RIGHT_TOUCH;
    } else {
        event.source = UI_UNKNOW;
    }
    event.value = ev.value;
    event.fd = fd;
    event.ev = ev;
    event.data = data;

    instance().put_pending_event(event);

    return 0;
}

void* UiCore::read_event(void *)
{
    while (instance().is_running()) {
        if (!ev_wait(-1)) {
            ev_dispatch();  // handle_event run in here.
        }
    }
    return NULL;
}

bool UiCore::merge_events(Event* result, const std::vector<Event>& events)
{
    if (events.empty()) return false;

#if 0
    std::map<uint64_t, int> dict;
    for (size_t i = 0; i < events.size(); i++) {
        Event e = events[i];
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

void UiCore::stop()
{
    stop_event_thread();
}

void UiCore::run(UiBase *current)
{
    switch_current_ui(current);
    start_event_thread();

    // main thread draw frames, merge and handle events etc ...
    struct timespec ts_start, ts_end;
    while (is_running()) {
        clock_gettime(CLOCK_REALTIME, &ts_start);

        UiBase *pCurrent = NULL, *pEnter = NULL, *pLeave = NULL;
        get_ui_pointers(&pLeave, &pCurrent, &pEnter);

        // check and fire enter/leave callback
        if (pLeave) {
            set_leave_ui(NULL);
            pLeave->OnLeave(); // may call SetCurrentUI
        }
        if (pEnter) {
            set_enter_ui(NULL);
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
        bool merged = merge_events(&event, get_pending_events());
        if (merged) {
            int type = event.ev.type;
            int value = event.ev.value;
            if (ignore_key_release() && (type == EV_KEY || type == EV_ABS) && value == 0) {
                ignore = true;
            }
        }
        if (merged && !ignore) {
            pCurrent->OnEvent(event.fd, &event.ev, event.data);
            switch (event.source) {
                case UI_POWER_KEY:
                case UI_TOP_KEY:
                    pCurrent->OnKey(event.ev.code, event.value);
                    break;
                case UI_LEFT_TOUCH:
                    pCurrent->OnLeftTouch(event.value);
                    break;
                case UI_RIGHT_TOUCH:
                    pCurrent->OnRightTouch(event.value);
                    break;
                case UI_UNKNOW:
                    break;
                default:
                    break;
            }
        }

        clock_gettime(CLOCK_REALTIME, &ts_end);

        long cost_us = diff_timespec_us(&ts_end, &ts_start);
        pCurrent->last_frame_cost_ = cost_us;
        long expected_cost_us = US_PER_S / get_expected_fps();
        long pad = expected_cost_us - cost_us;
        while (pad < 0) pad += expected_cost_us;
//        XLOGI("frame cost actual: %06d, expected %06d, pad: %06d", cost_us, expected_cost_us, pad);
        usleep(pad);
    }

    join_event_thread();
}

bool UiCore::is_running() const
{
    ScopedLock _lock(&mutex_);
    return running_;
}

int UiCore::get_expected_fps() const
{
    ScopedLock _lock(&mutex_);
    return expected_fps_;
}

bool UiCore::ignore_key_release() const
{
    ScopedLock _lock(&mutex_);
    return ignore_key_release_;
}

UiBase *UiCore::get_leave_ui() const
{
    ScopedLock _lock(&mutex_);
    return leave_ui_;
}

UiBase *UiCore::get_enter_ui() const
{
    ScopedLock _lock(&mutex_);
    return enter_ui_;
}

UiBase *UiCore::get_current_ui() const
{
    ScopedLock _lock(&mutex_);
    return current_ui_;
}

void UiCore::set_running(bool running)
{
    ScopedLock _lock(&mutex_);
    running_ = running;
}

void UiCore::set_expected_fps(int fps)
{
    ScopedLock _lock(&mutex_);
    expected_fps_ = fps;
}

void UiCore::ignore_key_release(bool ignore)
{
    ScopedLock _lock(&mutex_);
    ignore_key_release_ = ignore;
}

void UiCore::set_leave_ui(UiBase *ui)
{
    ScopedLock _lock(&mutex_);
    leave_ui_ = ui;
}

void UiCore::set_enter_ui(UiBase *ui)
{
    ScopedLock _lock(&mutex_);
    enter_ui_ = ui;
}

void UiCore::set_current_ui(UiBase *ui)
{
    ScopedLock _lock(&mutex_);
    current_ui_ = ui;
}

void UiCore::switch_current_ui(UiBase *ui)
{
    ScopedLock _lock(&mutex_);
    leave_ui_ = current_ui_;
    current_ui_ = ui;
    enter_ui_ = ui;
}

void UiCore::get_ui_pointers(UiBase **leave, UiBase **current, UiBase **enter)
{
    ScopedLock _lock(&mutex_);
    if (leave) *leave = leave_ui_;
    if (current) *current = current_ui_;
    if (enter) *enter = enter_ui_;
}

