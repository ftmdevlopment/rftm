//
// Created by dev on 16-12-3.
//

#ifndef RFTM_UI_MAIN_H
#define RFTM_UI_MAIN_H

#include "ui_base.h"
#include "ui_test.h"
#include "utils.h"

#include "qrcode/QrCode.hpp"

using qrcodegen::QrCode;

class UiMain : public UiBase
{
public:
    UiMain();

    ~UiMain();

protected:
    static const int kCases = 21;

    void Draw();

    void OnKey(int value);

    void OnLeftTouch(int value);

    void OnRightTouch(int value);

    void OnAlarm();

    void OnEnter();

    void OnLeave();

private:
    void draw_qrcode();

private:
    QrCode qrCode_;

    color_t bg_color = {0x00, 0x00, 0x00, 0xff};
    color_t border_color = {0xb0, 0x00, 0x00, 0xff};
    color_t gap_color  = {0x00, 0xa0, 0x00, 0xff};
    color_t main_color = {0x00, 0x10, 0x48, 0xff};
    color_t case_color = {0x66, 0x66, 0x66, 0xff};
    color_t font_color = {0xff, 0xff, 0xff, 0xff};
    color_t arrow_color = {0x7f, 0x7f, 0x7f, 0xff};
    color_t focus_color = {0xa0, 0xa0, 0xa0, 0xff};
    color_t qr_bg_color = {0xff, 0xff, 0xff, 0xff};
    color_t qr_fg_color = {0x00, 0x00, 0x00, 0xff};
    color_t pass_color = {0x00, 0xff, 0x00, 0xff};
    color_t fail_color = {0xff, 0x00, 0x00, 0xff};

    point_t center;

    int border_radius;
    int gap_radius;
    int main_radius;
    int case_radius;
    int case_center_radius;
    point_t case_centers[kCases];

    color_t* case_colors[kCases];

    int arrow_end_radius;
    triangle_t arrow_places[kCases];

    int focus_case_id;
    UiTest* tests_[kCases];
};


#endif // RFTM_UI_MAIN_H
