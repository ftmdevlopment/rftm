//
// Created by dev on 16-12-3.
//

#include <math.h>
#include <stdio.h>

#include "log.h"
#include "utils.h"
#include "common.h"
#include "minui/minui.h"
#include "qrcode/QrCode.hpp"
#include "ui_main.h"
#include "test/empty_test.h"
#include "test/version_test.h"
#include "test/fan_test.h"
#include "test/key_test.h"
#include "test/led_test.h"
#include "test/light_sensor_test.h"
#include "test/speaker_test.h"
#include "test/dlp_test.h"
#include "test/mem_test.h"
#include "test/wifi_test.h"
#include "test/battery_test.h"
#include "test/clear_flash.h"
#include "test/power_off.h"
#include "test/ftm_exit.h"
#include "test/mic_test.h"
#include "test/camera_test.h"

using qrcodegen::QrCode;

static const char* kVersion = FTM_VERSION;

static const int kResultLineMaxChars = 20;

static const double kFreezeAngle = (80/180.0*M_PI);

static const int kBorderWidth = 14;
static const int kGapperWidth = 16;
static const int kCaseRadius  = 18;
static const int kArrowLength = 16;
static const double kArrowAngle  = (45/180.0*M_PI);
static const int kFontLineDist = 4;

static const int kQRCellSize = 4;
static const int kQRBorderWidth = 8;

//static const char* kDefaultQrText = "846425A00481,010116000978,0a22,2.1.3,111,212000010000000000,,,00000bf0";
static const char* kDefaultSN2 = "012345678901";
static const char* kDefaultSN3 = "012345678901";

static char state2result(int state)
{
    switch (state) {
        case UiTest::TS_INIT:
            return '0';
        case UiTest::TS_FAILED:
            return '1';
        case UiTest::TS_PASSED:
            return '2';
        default:
            return '0';
    }
}

static std::string get_serial()
{
    std::string out;
    run_command("getprop ro.serialno", &out);
    return trim_string(out);
}

void UiMain::draw_main()
{
    // test state, name, result
    set_color(&font_color);
    fill_text(state_pos_.x, state_pos_.y, tests_[focus_case_id]->state_str(), 0);
    fill_text(name_pos_.x, name_pos_.y, tests_[focus_case_id]->name(), 0);

    // split to lines
    std::string lines[kResultLines];
    std::string result = tests_[focus_case_id]->result();
    split_string(lines, kResultLines, result, '\n', kResultLineMaxChars);

    // draw each line.
    for (int i = 0; i < kResultLines; i++) {
        fill_text(result_pos_[i].x, result_pos_[i].y, lines[i].c_str(), 0);
    }

    // QR code
    set_color(&qr_bg_color); // background
    gr_fill(qrbase_pos_.x, qrbase_pos_.y, qrbase_pos_.x + qrbase_size_, qrbase_pos_.y + qrbase_size_);
    set_color(&qr_fg_color); // foreground

    // update qr data
    std::string test_results;
    for (int i = 0; i < kCases; i++) {
        test_results += state2result(tests_[i]->state());
    }
    qrdata_.set_results(test_results);
    qrdata_.update_checksum();

    QrCode qrCode = QrCode::encodeText(qrdata_.to_string().c_str(), QrCode::Ecc::LOW);

    // recalculate qr position
    qrsize_ = qrCode.size;
    int size = qrsize_ * kQRCellSize + 2 * kQRBorderWidth;
    qrbase_size_ = size;
    static int W = gr_fb_width();
    static int H = gr_fb_height();
    qrbase_pos_.x = (W - size)/2;
    qrbase_pos_.y = (H - size)/2;
    qrcode_pos_.x = qrbase_pos_.x + kQRBorderWidth;
    qrcode_pos_.y = qrbase_pos_.y + kQRBorderWidth;
    int fw, fh;
    gr_font_size(&fw, &fh);
    name_pos_.x = W/2;
    name_pos_.y = qrbase_pos_.y - fh/2 - kFontLineDist;
    state_pos_.x = W/2;
    state_pos_.y = qrbase_pos_.y - (fh/2 + fh) - kFontLineDist;

    for (int i = 0; i < kResultLines; i++) {
        result_pos_[i].x = W/2;
        result_pos_[i].y = qrbase_pos_.y + size + i*(fh + kFontLineDist) + (fh/2) + kFontLineDist;
    }

    for (int i = 0; i < qrsize_; i++) {
        for (int j = 0; j < qrsize_; j++) {
            if (qrCode.getModule(i, j)) {
                int x = qrcode_pos_.x + i*kQRCellSize;
                int y = qrcode_pos_.y + j*kQRCellSize;
                gr_fill(x, y, x + kQRCellSize, y + kQRCellSize);
            }
        }
    }
}

UiMain::UiMain()
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
    arrow_end_radius = case_center_radius - 1.25*kCaseRadius;
    focus_case_id = 0;

    triangle_t t0;
    point_t* c = case_centers;
    triangle_t* t = arrow_places;
    t0.a.x = x0;
    t0.b.x = x0 + kArrowLength*tan(kArrowAngle/2);
    t0.c.x = x0 - kArrowLength*tan(kArrowAngle/2);
    t0.a.y = y0 + arrow_end_radius;
    t0.b.y = t0.a.y - kArrowLength;
    t0.c.y = t0.b.y;
    for (i = 0; i < kCases; i++) {
//        double a = (M_PI/6) + (i+0.5) * ((2*M_PI - (M_PI/6)*2)/kCases);
        double a = kFreezeAngle/2 + (i + 0.5) * (2*M_PI - kFreezeAngle)/kCases;
//        double a = i * (2*M_PI)/kCases;
        c[i].x = x0 - case_center_radius * sin(a);
        c[i].y = y0 + case_center_radius * cos(a);
        printf("a: %.3f*M_PI, %3.1f ", a/M_PI, a/M_PI*180.0);
        printf("c[%d] = {%d, %d}\n", i, c[i].x, c[i].y);

        rotate_with(&t[i].a, &t0.a, &center, -a);
        rotate_with(&t[i].b, &t0.b, &center, -a);
        rotate_with(&t[i].c, &t0.c, &center, -a);

        case_colors[i] = &case_color;
    }

    qrdata_ = QrData::Builder::instance()
            .set_sn1(get_serial())
            .set_sn2(kDefaultSN2)
            .set_sn3(kDefaultSN3)
            .set_version(kVersion)
            .build();

    set_alarm(1);
    memset(tests_, 0, sizeof(tests_));

    int count = 0;
    tests_[count++] = new VersionTest(this, "Version test");
    tests_[count++] = new MemTest(this, "Memory test");
    tests_[count++] = new WifiTest(this, "WiFi test");
    tests_[count++] = new DlpTest(this, "DLP test");
    tests_[count++] = new LedTest(this, "Led test");
    tests_[count++] = new CameraTest(this, "Camera test");
    tests_[count++] = new LightSensorTest(this, "Light sensor test");
    tests_[count++] = new FanTest(this, "Fan test");
    tests_[count++] = new KeyTest(this, "Key test");
    tests_[count++] = new SpeakerTest(this, "Speaker test");
    tests_[count++] = new MicTest(this, "Mic test");
    tests_[count++] = new BatteryTest(this, "Battery test");
    tests_[count++] = new ClearFlash(this, "Clear flash");
    tests_[count++] = new FtmExit(this, "FTM exit");
    tests_[count++] = new PowerOff(this, "Power off");
    for (int i = count; i < kCases; i++) {
        tests_[i] = new EmptyTest(this, "");
        case_colors[i] = &empty_color;
    }

    printf("border_radius: %d\n", border_radius);
    printf("gap_radius: %d\n", gap_radius);
    printf("main_radius: %d\n", main_radius);
    printf("case_radius: %d\n", case_radius);
    printf("case_center_radius: %d\n", case_center_radius);
}

UiMain::~UiMain()
{
    for (int i = 0; i < kCases; i++) {
        delete tests_[i];
    }
}

void UiMain::Draw()
{
    const int x0 = center.x;
    const int y0 = center.y;
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
    draw_main();

    // cases
    const point_t* c = case_centers;
    for (int i = 0; i < kCases; i++) {
        switch (tests_[i]->state()) {
            case UiTest::TS_PASSED:
                set_color(&pass_color);
                break;
            case UiTest::TS_FAILED:
                set_color(&fail_color);
                break;
            default:
                set_color(case_colors[i]);
                break;
        }

        fill_circle(c[i].x, c[i].y, case_radius);

        char text[64];
        snprintf(text, sizeof(text), "%d", i+1);
        set_color(&font_color);
        fill_text(c[i].x, c[i].y, text, 1);
    }

    // arrow
    set_color(&arrow_color);
    fill_triangle(&arrow_places[focus_case_id]);

    gr_flip();
}

void UiMain::OnLeftTouch(int value)
{
    focus_case_id = (kCases + focus_case_id - 1) % kCases;
    XLOGI("focus: %d", focus_case_id);
}

void UiMain::OnRightTouch(int value)
{
    focus_case_id = (focus_case_id + 1) % kCases;
    XLOGI("focus: %d", focus_case_id);
}

void UiMain::OnKey(int code, int value)
{
    if (code != KEY_OK) return;
    XLOGI("switch to test %d\n", focus_case_id);
    UiBase::SetCurrentUI(tests_[focus_case_id]);
}

void UiMain::OnAlarm()
{
    XLOGI("alarm %ld", clock());
    set_alarm(1);
}

void UiMain::OnEnter()
{
    XLOGI("enter %p", this);
}

void UiMain::OnLeave()
{
    XLOGI("leave %p", this);
}
