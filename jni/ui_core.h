//
// Created by xu on 17-2-11.
//

#ifndef RFTM_UI_CORE_H
#define RFTM_UI_CORE_H

#include <pthread.h>
#include <linux/input.h>

#include <stdint.h>

#include "blocking_queue.h"

class UiBase;  // forward declaration

class UiCore
{
public:
    // singleton usage only
    static UiCore& instance();

    void run(UiBase *current);
    void stop();

    // for UI pointers
    void switch_current_ui(UiBase *ui);
    void get_ui_pointers(UiBase **leave, UiBase **current, UiBase **enter);

    // getters
    UiBase* get_leave_ui() const;
    UiBase* get_enter_ui() const;
    UiBase* get_current_ui() const;
    bool is_running() const;
    int get_expected_fps() const;
    bool ignore_key_release() const;

    // setters
    void set_leave_ui(UiBase* ui);
    void set_enter_ui(UiBase* ui);
    void set_current_ui(UiBase* ui);
    void set_running(bool running);
    void set_expected_fps(int fps);
    void ignore_key_release(bool ignore);

private:
    // singleton usage only
    UiCore();
    ~UiCore();

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

    // for event thread
    inline void start_event_thread();
    inline void join_event_thread();
    inline void stop_event_thread();

    // for event handling
    inline std::vector<Event> get_pending_events();
    inline void put_pending_event(Event event);

    static int handle_event(int fd, uint32_t epevents, void *data);
    static bool merge_events(Event* result, const std::vector<Event>& events);
    static void* read_event(void*);

private:
    pthread_t event_thread_;
    BlockingQueue<Event> event_queue_{1024};

    mutable pthread_mutex_t mutex_;
    UiBase* leave_ui_ = NULL;   // @guard by mutex_
    UiBase* enter_ui_ = NULL;   // @guard by mutex_
    UiBase* current_ui_ = NULL; // @guard by mutex_

    bool running_ = false;
    bool ignore_key_release_ = false;
    int expected_fps_ = 50;
};


#endif //RFTM_UI_CORE_H
