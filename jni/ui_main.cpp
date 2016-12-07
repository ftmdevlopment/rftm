//
// Created by dev on 16-12-3.
//

#include <math.h>
#include <stdio.h>

#include "utils.h"
#include "minui/minui.h"
#include "ui_main.h"
#include "ui_test.h"
#include "test/draw_test.h"

#include "qrcode/QrCode.hpp"

//#define BORDER 12 // border width
//#define GAPWID 20 // gap width
//#define RACASE 18 // case radius
#define ARROWH 25 // arrow height
#define ARROWA (M_PI/12) // arrow angle(rad)

static const double kFreezeAngle = (80/180.0*M_PI);

static const int kBorderWidth = 14;
static const int kGapperWidth = 16;
static const int kCaseRadius  = 18;

static const int kQRCellSize = 4;
static const int kQRBorderWidth = 8;

static const char* kDefaultQrText = "846425A00481,010116000978,0a22,2.1.3,111,212000010000000000,,,00000bf0";

void UiMain::draw_qrcode()
{
    int W, H, x0, y0, x1, y1, size;

    W = gr_fb_width();
    H = gr_fb_height();

    size = qrCode_.size * kQRCellSize + 2 * kQRBorderWidth;
    x0 = (W - size)/2;
    y0 = (H - size)/2;
    x1 = x0 + kQRBorderWidth;
    y1 = y0 + kQRBorderWidth;

    set_color(&qr_bg_color); // background
    gr_fill(x0, y0, x0 + size, y0 + size);
    set_color(&qr_fg_color); // foreground
    for (int i = 0; i < qrCode_.size; i++) {
        for (int j = 0; j < qrCode_.size; j++) {
            if (qrCode_.getModule(i, j)) {
                int x = x1 + i*kQRCellSize;
                int y = y1 + j*kQRCellSize;
                gr_fill(x, y, x + kQRCellSize, y + kQRCellSize);
            }
        }
    }
}

UiMain::UiMain()
    : qrCode_(QrCode::encodeText(kDefaultQrText, QrCode::Ecc::LOW))
{
    int i, x0, y0, W, H;

    W = gr_fb_width();
    H = gr_fb_height();
    x0 = center.x = W/2;
    y0 = center.y = H/2;

    border_radius = min(W/2, H/2);
    gap_radius    = border_radius - kBorderWidth;
    main_radius   = gap_radius    - kGapperWidth;
    case_radius   = kCaseRadius;
    case_center_radius = main_radius - 1.618f*kCaseRadius;
    arrow_end_radius = case_center_radius - 1.618*kCaseRadius;
    focus_case_id = 0;

    triangle_t t0;
    point_t* c = case_centers;
    triangle_t* t = arrow_places;
    t0.a.x = x0;
    t0.b.x = x0 + ARROWH*tan(ARROWA/2);
    t0.c.x = x0 - ARROWH*tan(ARROWA/2);
    t0.a.y = y0 + arrow_end_radius;
    t0.b.y = t0.a.y - ARROWH;
    t0.c.y = t0.b.y;
    for (i = 0; i < kCases; i++) {
//        double a = (M_PI/6) + (i+0.5) * ((2*M_PI - (M_PI/6)*2)/kCases);
        double a = kFreezeAngle/2 + (i + 0.5) * (2*M_PI - kFreezeAngle)/kCases;
//        double a = i * (2*M_PI)/kCases;
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

    set_alarm(1);
    memset(tests_, 0, sizeof(tests_));

    for (int i = 0; i < kCases; i++) {
        tests_[i] = new DrawTest(this, "DrawTest");
    }
}

UiMain::~UiMain()
{
    for (int i = 0; i < kCases; i++) {
        delete tests_[i];
    }
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

    // QR code
    draw_qrcode();

    // cases
    c = case_centers;
    for (i = 0; i < kCases; i++) {
        switch (tests_[i]->state()) {
            case UiTest::TS_PASSED:
                set_color(&pass_color);
                break;
            case UiTest::TS_FAILED:
                set_color(&fail_color);
                break;
            default:
                set_color(&case_color);
        }
        if (i == focus_case_id) {
            set_color(&focus_color);
        }
        fill_circle(c[i].x, c[i].y, case_radius);

        char text[64];
        snprintf(text, sizeof(text), "%d", i);
        set_color(&font_color);
        fill_text(c[i].x, c[i].y, text, 1);
    }

    gr_flip();
}

void UiMain::OnLeftTouch(int value)
{
    focus_case_id = (kCases + focus_case_id - 1) % kCases;
    gr_info("focus: %d", focus_case_id);
}

void UiMain::OnRightTouch(int value)
{
    focus_case_id = (focus_case_id + 1) % kCases;
    gr_info("focus: %d", focus_case_id);
}

void UiMain::OnKey(int value)
{
    gr_info("switch to test %d\n", focus_case_id);
    UiBase::SetCurrentUI(tests_[focus_case_id]);
}

void UiMain::OnAlarm()
{
    gr_info("alarm %d", clock());
    set_alarm(1);
}

void UiMain::OnEnter()
{
    gr_info("enter %p", this);
}

void UiMain::OnLeave()
{
    gr_info("leave %p", this);
}
