//
// Created by dev on 16-12-9.
//

#include "dlp_test.h"

void DlpTest::Draw()
{
    std::string img;
    if (display_queue_.peek(&img)) {
        XLOGI("got %s", img.c_str());
        gr_color(0, 0, 0, 255); gr_clear();
        fill_image(gr_fb_height()/2, gr_fb_height()/2, img.c_str());
        gr_flip();
    }
}

void DlpTest::RunTest()
{
    clear_judge_result();
    wait_for_judge_result();

    clear_judge_result();
}

void DlpTest::scan_images()
{
#ifdef FTM_IMG_PATH
    std::string dir = FTM_IMG_PATH;
#else  // FTM_IMG_PATH
    std::string dir = "/data/local/tmp/";
#endif  // FTM_IMG_PATH
    std::vector<std::string> vec;
    list_directory(&vec, dir.c_str());

    std::sort(vec.begin(), vec.end());

    auto start_with = [](const std::string& str, std::string keyword) {
        return str.find(keyword) == 0;
    };
    auto end_with = [](const std::string& str, std::string keyword) {
        auto pos = str.find(keyword);
        return pos != str.npos && pos + keyword.size() == str.size();
    };

    std::for_each(vec.begin(), vec.end(), [&](std::string& s) {
        if (start_with(s, "dlp_")  // start with dlp_
            && end_with(s, ".png")) { // end with .png
            s = s.substr(0, s.find_last_of('.'));
            XLOGI("found img %s", s.c_str());
        }
    });

    std::remove_if(vec.begin(), vec.end(), [](std::string& s) {
        return s.size() == 0 || (s.size() && s[0] == '.');
    });

    images_.swap(vec);
    XLOGI("images: %d", images_.size());
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
    if (queued_image_index_ < images_.size()) {
        std::string img = images_[queued_image_index_];
        if (img.size() > 0) {
            XLOGI("put %s", img.c_str());
            display_queue_.put(img);
        }
        queued_image_index_++;
    }
}

bool DlpTest::has_next_image()
{
    return queued_image_index_ < images_.size();
}

void DlpTest::OnEnter()
{
    UiTest::OnEnter();

    scan_images();
    queued_image_index_ = 0;

    next_image();
}
