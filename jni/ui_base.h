//
// Created by dev on 16-12-3.
//

#ifndef RFTM_UI_BASE_H
#define RFTM_UI_BASE_H

#include <pthread.h>
#include <stdint.h>

class UiBase
{
public:
    UiBase();

    virtual ~UiBase();

    void run();

    void suspend();

    void resume();

protected:
    static const int UI_KEY_UP = 0;
    static const int UI_KEY_DOWN = 1;

    virtual void Draw() = 0;

    virtual void OnKey(int code, int value) = 0;

    virtual void OnLeftTouch(int code, int value) = 0;

    virtual void OnRightTouch(int code, int value) = 0;

    virtual void OnSuspend() {}

    virtual void OnResume() {}

private:

    static int event_callback(int fd, uint32_t epevents, void *data);

    static void* read_event(void*);
    static pthread_t event_reader;

    UiBase *last_ui_;  // make Ui stackable
};


#endif // RFTM_UI_BASE_H
