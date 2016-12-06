//
// Created by dev on 16-12-3.
//

#ifndef RFTM_UI_BASE_H
#define RFTM_UI_BASE_H

#include <pthread.h>
#include <stdint.h>

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
    };

    // for event source:
    static const uint16_t UI_POWER_KEY = 1;
    static const uint16_t UI_LEFT_TOUCH = 2;
    static const uint16_t UI_RIGHT_TOUCH = 4;
    static const uint16_t UI_UNKNOW = 0;

    // for common settings init:
    static void SetIgnoreRelease(bool);
    static void SetExpectedFPS(int);

    // for runtime usage:
    static void SetRunning(bool);
    static void SetCurrentUI(UiBase*);
    static UiBase* GetCurrentUI();

protected:
    virtual void Draw() = 0;

    virtual void OnKey(int value);

    virtual void OnLeftTouch(int value);

    virtual void OnRightTouch(int value);

    virtual void OnAlarm();

    // calling when enter this UI
    virtual void OnEnter();

    // calling when leave this UI
    virtual void OnLeave();

    long last_frame_cost_; // last frame time consumption in microsecond(us).

private:
    static bool s_ignore_release;
    static bool s_running;
    static int s_fps_expected;
    static int event_callback(int fd, uint32_t epevents, void *data);

    struct timespec last_alarm_ts_;
    long alarm_ms_;
};


#endif // RFTM_UI_BASE_H
