//
// Created by dev on 16-12-9.
//

#include "dlp_test.h"

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
    current_displayed_ = 0;

    scan_images();
    next_image();

    clear_judge_result();
    wait_for_judge_result();

    clear_judge_result();
}

void DlpTest::scan_images()
{
    {
        ScopedLock lock(&mutex_);
        if (images_.size() > 0) return;
    }

#ifdef FTM_IMG_PATH
    std::string dir = FTM_IMG_PATH;
#else  // FTM_IMG_PATH
    std::string dir = "/data/local/tmp/";
#endif  // FTM_IMG_PATH
    std::vector<std::string> vec, out;
    list_directory(&vec, dir.c_str());

    std::sort(vec.begin(), vec.end());

    auto start_with = [](const std::string& str, std::string keyword) {
        return str.find(keyword) == 0;
    };
    auto end_with = [](const std::string& str, std::string keyword) {
        auto pos = str.find(keyword);
        return pos != str.npos && pos + keyword.size() == str.size();
    };
    for (auto& s: vec) {
        XLOGI("scan %s ...", s.c_str());
        if (start_with(s, "dlp_")  // start with dlp_
            && end_with(s, ".png")) { // end with .png
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

void DlpTest::OnRightTouch(int value)
{
    if (in_right_filter(value)) return;
    if (has_next_image()) {
        next_image();
    } else {
        pass();
        UiUserJudgeTest::OnRightTouch(value);
    }
}

void DlpTest::next_image()
{
    std::string img;
    {
        ScopedLock lock(&mutex_);
        if (current_displayed_ < images_.size()) {
            img = images_[current_displayed_];
            XLOGI("put %s", img.c_str());
            current_displayed_++;
        }
    }
    if (img.size() > 0) {
        show_.put(img);
    }
}

bool DlpTest::has_next_image()
{
    ScopedLock lock(&mutex_);
    return current_displayed_ < images_.size();
}
