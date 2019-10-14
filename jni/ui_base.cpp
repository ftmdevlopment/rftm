//
// Created by dev on 16-12-3.
//

#include <time.h>

#include "log.h"
#include "ui_base.h"
#include "ui_core.h"
#include "minui/minui.h"

void UiBase::SetExpectedFPS(int fps)
{
    UiCore::instance().set_expected_fps(fps);
}

int UiBase::GetExpectedFPS()
{
    return UiCore::instance().get_expected_fps();
}

void UiBase::SetIgnoreRelease(bool ignore)
{
    UiCore::instance().ignore_key_release(ignore);
}

void UiBase::SetCurrentUI(UiBase *ui)
{
    UiCore::instance().switch_current_ui(ui);
}

UiBase* UiBase::GetCurrentUI()
{
    return UiCore::instance().get_current_ui();
}


UiBase::UiBase()
{
}

UiBase::~UiBase()
{
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
    UiCore::instance().run(this);
}

void UiBase::stop()
{
    UiCore::instance().stop();
}

double UiBase::get_last_alarm_ts()
{
    return last_alarm_ts_.tv_sec + last_alarm_ts_.tv_nsec/1e9;
}

long UiBase::get_last_frame_cost()
{
    return last_frame_cost_;
}

