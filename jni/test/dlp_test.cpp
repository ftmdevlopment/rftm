//
// Created by dev on 16-12-9.
//

#include "dlp_test.h"

void DlpTest::Draw()
{
    std::string img;
    if (images_.peek(&img)) {
        XLOGI("got %s", img.c_str());
        gr_color(0, 0, 0, 255); gr_clear();
        fill_image(gr_fb_height()/2, gr_fb_height()/2, img.c_str());
    }
}

void DlpTest::RunTest()
{
    std::string dir = "/res/images/";
    std::vector<std::string> vec;
    list_directory(&vec, dir.c_str());

    std::sort(vec.begin(), vec.end());
    set_alarm(vec.size() * kDiplayTime);
    for (auto& s: vec) {
        if (s.find("dlp_") == 0) {
            std::string base = s.substr(0, s.find_last_of('.'));
            XLOGI("put %s", base.c_str());
            images_.put(base);
            sleep(kDiplayTime);
        }
    }
}
