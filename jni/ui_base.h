//
// Created by dev on 16-12-3.
//

#ifndef RFTM_UI_BASE_H
#define RFTM_UI_BASE_H

#include <pthread.h>
#include <stdint.h>
#include <linux/input.h>

class UiBase
{
    friend struct GlobalConext;
public:
    UiBase();

    virtual ~UiBase();

    void run();

    void set_alarm(long s);

    void set_alarm_ms(long ms);

    struct Event {
        uint16_t source;
        uint32_t value;
        int fd;
        struct input_event ev;
        void* data;
    };

    // for event source:
    static const uint16_t UI_TOP_KEY = 1;
    static const uint16_t UI_POWER_KEY = 2;
    static const uint16_t UI_LEFT_TOUCH = 0x100;
    static const uint16_t UI_RIGHT_TOUCH =0x200;
    static const uint16_t UI_UNKNOW = 0;

    // for common settings init:
    static void SetIgnoreRelease(bool);
    static void SetExpectedFPS(int);

    // for runtime usage:
    static void SetRunning(bool);
    static void SetCurrentUI(UiBase*);
    static UiBase* GetCurrentUI();

    virtual void Draw() = 0;

protected:
    virtual void OnEvent(int fd, struct input_event* ev, void* data);

    virtual void OnKey(int code, int value);

    virtual void OnLeftTouch(int value);

    virtual void OnRightTouch(int value);

    virtual void OnAlarm();

    // calling when enter this UI
    virtual void OnEnter();

    // calling when leave this UI
    virtual void OnLeave();

    long last_frame_cost_; // last frame time consumption in microsecond(us).

    double get_last_alarm_ts();
private:
    static bool s_ignore_release;
    static bool s_running;
    static int s_fps_expected;
    static int event_callback(int fd, uint32_t epevents, void *data);

    struct timespec last_alarm_ts_;
    long alarm_ms_;
};


#endif // RFTM_UI_BASE_H
