//
// Created by dev on 16-12-3.
//

#ifndef RFTM_UI_MAIN_H
#define RFTM_UI_MAIN_H

#include "ui_base.h"
#include "utils.h"

#define NCASES 21 // number cases
#define BORDER 25 // border width
#define GAPWID 25 // gap width
#define RACASE 18 // case radius
#define ARROWH 25 // arrow height
#define ARROWA (M_PI/12) // arrow angle(rad)

class UiMain : public UiBase
{
public:
    static const int kCases = 21;

    UiMain();

protected:
    void Draw();

    void OnKey(int value);

    void OnLeftTouch(int value);

    void OnRightTouch(int value);

    void OnAlarm();

private:
    color_t bg_color = {0x00, 0x00, 0x00, 0xff};
    color_t border_color = {0x00, 0x00, 0xff, 0xff};
    color_t gap_color = {0x00, 0xb3, 0x80, 0xff};
    color_t main_color = {0xff, 0xff, 0xff, 0xff};
    color_t case_color = {0xff, 0x94, 0x4d, 0xff};
    color_t font_color = {0x00, 0x00, 0x00, 0xff};
    color_t arrow_color = {0x7f, 0x7f, 0x7f, 0xff};
    color_t focus_color = {0xa0, 0xa0, 0xa0, 0xff};

    point_t center;
    int border_radius, gap_radius, main_radius;

    int focus_case_id;
    int case_center_radius;
    int case_radius = RACASE;
    point_t case_centers[NCASES];
    color_t* case_colors[NCASES];

    int arrow_end_radius;
    triangle_t arrow_places[NCASES];

    UiBase* case1_test_;
};


#endif // RFTM_UI_MAIN_H
