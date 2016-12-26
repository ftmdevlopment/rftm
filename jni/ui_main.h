//
// Created by dev on 16-12-3.
//

#ifndef RFTM_UI_MAIN_H
#define RFTM_UI_MAIN_H

#include "utils.h"
#include "ui_base.h"
#include "ui_test.h"
#include "qr_data.h"

class UiMain : public UiBase
{
    friend class UiTest;
public:
    UiMain();

    ~UiMain();

protected:
    static const int kCases = 20;
    static const int kResultLines = 5;

    void Draw();

    void OnKey(int code, int value);

    void OnLeftTouch(int value);

    void OnRightTouch(int value);

    void OnAlarm();

    void OnEnter();

    void OnLeave();

private:
    void draw_main();

private:
    color_t bg_color = {0x00, 0x00, 0x00, 0xff};
    color_t border_color = {0xb0, 0x00, 0x00, 0xff};
    color_t gap_color  = {0x00, 0xa0, 0x00, 0xff};
    color_t main_color = {0x00, 0x10, 0x48, 0xff};
    color_t font_color = {0xff, 0xff, 0xff, 0xff};
    color_t arrow_color = {0xa0, 0xa0, 0xa0, 0xff};

    // QR code colors
    color_t qr_bg_color = {0xff, 0xff, 0xff, 0xff};
    color_t qr_fg_color = {0x00, 0x00, 0x00, 0xff};

    // case colors
    color_t case_color = {0x66, 0x66, 0x66, 0xff};
    color_t empty_color = {0x40, 0x40, 0x40, 0xff};
    color_t pass_color = {0x00, 0xff, 0x00, 0xff};
    color_t fail_color = {0xff, 0x00, 0x00, 0xff};
    color_t testing_color = {0xff, 0xff, 0x00, 0xff};

    point_t center;

    int border_radius;
    int gap_radius;
    int main_radius;
    int case_radius;
    int case_center_radius;
    point_t case_centers[kCases];
    point_t name_pos_;
    point_t state_pos_;
    point_t qrbase_pos_;
    point_t qrcode_pos_;
    int qrbase_size_;

    int qrsize_;
    QrData qrdata_;

    point_t result_pos_[kResultLines];

    color_t* case_colors[kCases];

    int arrow_end_radius;
    triangle_t arrow_places[kCases];

    int focus_case_id;
    UiTest* tests_[kCases];
};


#endif // RFTM_UI_MAIN_H
