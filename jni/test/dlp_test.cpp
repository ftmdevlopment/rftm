//
// Created by dev on 16-12-9.
//

#include "dlp_test.h"

struct ScopedLock
{
    ScopedLock(pthread_mutex_t* mutex) : mutex_(mutex) { pthread_mutex_lock(mutex_); }
    ~ScopedLock() { pthread_mutex_unlock(mutex_); }
private:
    pthread_mutex_t* mutex_;
};


void DlpTest::Draw()
{
    std::string img;
    if (show_.peek(&img)) {
        XLOGI("got %s", img.c_str());
        gr_color(0, 0, 0, 255); gr_clear();
        fill_image(gr_fb_height()/2, gr_fb_height()/2, img.c_str());
        gr_flip();
    }
}

void DlpTest::RunTest()
{
    set_alarm(0);
    XLOGI("...");
    scan_images();
    next_image();
    current_displayed_ = 0;

    wait_for_judge_result();
    set_alarm_ms(1);
    clear_judge_result();
}

void DlpTest::scan_images()
{
    {
        ScopedLock lock(&mutex_);
        if (images_.size() > 0) return;
    }

#ifdef RESOURCE_PATH
    std::string dir = RESOURCE_PATH "img/";
#else  // RESOURCE_PATH
    std::string dir = "img/";
#endif  // RESOURCE_PATH
    std::vector<std::string> vec, out;
    list_directory(&vec, dir.c_str());

    std::sort(vec.begin(), vec.end());
    set_alarm(0);
    for (auto& s: vec) {
        if (s.find("dlp_") == 0) {
            std::string base = s.substr(0, s.find_last_of('.'));
            XLOGI("found img %s", base.c_str());
            out.push_back(base);
        }
    }

    {
        ScopedLock lock(&mutex_);
        images_.swap(out);
        XLOGI("size: %d", images_.size());
    }
}

void DlpTest::OnKey(int code, int value)
{
    if (code == KEY_OK) {
        next_image();
    }
}

void DlpTest::next_image()
{
    std::string img;
    if (current_displayed_ < images_.size()) {
        ScopedLock lock(&mutex_);
        img = images_[current_displayed_];
        XLOGI("put %s", img.c_str());
        current_displayed_++;
    }
    if (img.size() > 0) {
        show_.put(img);
    }
}
