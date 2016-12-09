//
// Created by dev on 16-12-9.
//

#include "dlp_test.h"

void DlpTest::Draw()
{
    for (;;) {
        std::string img = images_.take();
        if (done_) break;
        XLOGI("got %s", img.c_str());
        fill_image(gr_fb_height()/2, gr_fb_height()/2, img.c_str());
        gr_flip();
    }
}

void DlpTest::RunTest()
{
    std::string dir = "/res/images/";
    std::vector<std::string> vec;
    list_directory(&vec, dir.c_str());

    std::sort(vec.begin(), vec.end());
    for (auto& s: vec) {
        if (s.find("dlp_") == 0) {
            std::string base = s.substr(0, s.size() - 4);
            XLOGI("put %s", base.c_str());
            images_.put(base);
            sleep(2);
        }
    }
    done_ = true;
    images_.put("");
}
