//
// Created by dev on 16-12-3.
//

#ifndef RFTM_UI_BASE_H
#define RFTM_UI_BASE_H

#include <linux/input.h>

class UiCore;  // forward declaration

class UiBase
{
    friend class UiCore;
public:
    UiBase();

    virtual ~UiBase();

    void run();

    void stop();

    void set_alarm(long s);

    void set_alarm_ms(long ms);

    // for common settings init:
    static void SetIgnoreRelease(bool);
    static void SetExpectedFPS(int);
    static int GetExpectedFPS();

    // for runtime usage:
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

    long get_last_frame_cost();

    double get_last_alarm_ts();

private:
    long last_frame_cost_; // last frame time consumption in microsecond(us).
    struct timespec last_alarm_ts_;
    long alarm_ms_;
};


#endif // RFTM_UI_BASE_H
