//
// Created by dev on 16-12-3.
//

#include <math.h>
#include <stdio.h>

#include "utils.h"
#include "minui/minui.h"
#include "ui_main.h"
#include "ui_test.h"

UiMain::UiMain()
{
    int i, x0, y0, W, H;
    point_t* c;
    triangle_t* t;
    triangle_t t0;

    W = gr_fb_width();
    H = gr_fb_height();
    x0 = center.x = W/2;
    y0 = center.y = H/2;

    border_radius = min(W/2, H/2);
    gap_radius    = border_radius - BORDER;
    main_radius   = gap_radius    - GAPWID;
    case_radius   = RACASE;
    case_center_radius = main_radius - 1.618f*RACASE;
    arrow_end_radius = case_center_radius - 1.618*RACASE;
    focus_case_id = 0;

    c = case_centers;
    t = arrow_places;
    t0.a.x = x0;
    t0.b.x = x0 + ARROWH*tan(ARROWA/2);
    t0.c.x = x0 - ARROWH*tan(ARROWA/2);
    t0.a.y = y0 + arrow_end_radius;
    t0.b.y = t0.a.y - ARROWH;
    t0.c.y = t0.b.y;
    for (i = 0; i < NCASES; i++) {
//        double a = (M_PI/12) + (i+0.5) * ((2*M_PI - (M_PI/12)*2)/NCASES);
        double a = i * (2*M_PI)/NCASES;
        c[i].x = x0 - case_center_radius * sin(a);
        c[i].y = y0 + case_center_radius * cos(a);
        printf("a: %.3f*M_PI, %3.1f ", a/M_PI, a/M_PI*180.0);
        printf("c[%d] = {%d, %d}\n", i, c[i].x, c[i].y);

        t[i].a.x = x0 - arrow_end_radius * sin(a);
        t[i].a.y = y0 - arrow_end_radius * cos(a);
        rotate_with(&t[i].a, &t0.a, &center, a);
        rotate_with(&t[i].b, &t0.b, &center, a);
        rotate_with(&t[i].c, &t0.c, &center, a);

        case_colors[i] = &case_color;
    }
    case_colors[focus_case_id] = &focus_color;
    printf("border_radius: %d\n", border_radius);
    printf("gap_radius: %d\n", gap_radius);
    printf("main_radius: %d\n", main_radius);
    printf("case_radius: %d\n", case_radius);
    printf("case_center_radius: %d\n", case_center_radius);

    case1_test_ = new UiTest(this);
    set_alarm(1000);
}

void UiMain::Draw()
{
    int i;
    const int x0 = center.x;
    const int y0 = center.y;
    const point_t* c;
    // clear background
    set_color(&bg_color);;
    gr_clear();

    // border
    set_color(&border_color);
    fill_circle(x0, y0, border_radius);

    // gap
    set_color(&gap_color);
    fill_circle(x0, y0, gap_radius);

    // main
    set_color(&main_color);
    fill_circle(x0, y0, main_radius);

    {
        char text[64];
        snprintf(text, sizeof(text), "frame cost% 3ld(ms)", last_frame_cost_/1000);
        set_color(&font_color);
        fill_text(x0, y0, text);
    }

    // cases
    c = case_centers;
    for (i = 0; i < NCASES; i++) {
        char text[64];
        set_color(case_colors[i]);
        fill_circle(c[i].x, c[i].y, case_radius);

        snprintf(text, sizeof(text), "%d", i);
        set_color(&font_color);
        fill_text(c[i].x, c[i].y, text);
    }

    gr_flip();
}

void UiMain::OnLeftTouch(int value)
{
    if (!value) return;
    case_colors[focus_case_id] = &case_color;

    focus_case_id = (NCASES + focus_case_id - 1) % NCASES;
    gr_info("focus: %d", focus_case_id);

    case_colors[focus_case_id] = &focus_color;
}

void UiMain::OnRightTouch(int value)
{
    if (!value) return;
    case_colors[focus_case_id] = &case_color;

    focus_case_id = (focus_case_id + 1) % NCASES;
    gr_info("focus: %d", focus_case_id);

    case_colors[focus_case_id] = &focus_color;
}

void UiMain::OnKey(int value)
{
    if (!value) return;
    if (focus_case_id == 1) {
        UiBase::SetCurrentUI(case1_test_);
        case1_test_->set_alarm(5000);
    }
}

void UiMain::OnAlarm()
{
    gr_info("alarm %d", clock());
    set_alarm(1000);
}
