//
// Created by dev on 16-12-3.
//

#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sstream>

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
#include "test/dlp_age_test.h"

using qrcodegen::QrCode;

static const std::string kResultDirectory = "/data/local/tmp/ftm_results";

static const char* kVersion = FTM_VERSION;

static const int kResultLineMaxChars = 21;

static const double kFreezeAngle = (80/180.0*M_PI);

static const int kBorderWidth = 4;
static const int kGapperWidth = 16;
static const int kCaseRadius  = 18;
static const int kArrowLength = 16;
static const double kArrowAngle  = (45/180.0*M_PI);
static const int kFontLineDist = 4;

static const int kQRCellSize = 4;
static const int kQRBorderWidth = 8;


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

void UiMain::draw_main()
{
    static int W = gr_fb_width();
    static int H = gr_fb_height();
    int FW = 0, FH = 0;
    gr_font_size(&FW, &FH);

    // update qr data
    std::string test_results;
    for (int i = 0; i < kCases; i++) {
        test_results += state2result(tests_[i]->state());
    }
    qrdata_.set_results(test_results);
    qrdata_.update_checksum();

    QrCode qrCode = QrCode::encodeText(qrdata_.to_string().c_str(), QrCode::Ecc::LOW);

    // recalculate positions
    int qr_size = qrCode.size;
    int qr_bg_width = qr_size * kQRCellSize + 2 * kQRBorderWidth;
    qr_bg_rect_.x = (W - qr_bg_width)/2;
    qr_bg_rect_.y = (H - qr_bg_width)/2;
    qr_bg_rect_.w = qr_bg_width;
    qr_bg_rect_.h = qr_bg_width;
    qrcode_pos_.x = qr_bg_rect_.x + kQRBorderWidth;
    qrcode_pos_.y = qr_bg_rect_.y + kQRBorderWidth;

    name_pos_.x = W/2;
    name_pos_.y = qr_bg_rect_.y - FH/2 - kFontLineDist;
    state_pos_.x = W/2;
    state_pos_.y = qr_bg_rect_.y - (FH/2 + FH) - kFontLineDist;
    for (int i = 0; i < kResultLines; i++) {
        result_pos_[i].x = W/2;
        result_pos_[i].y = qr_bg_rect_.y + qr_bg_rect_.h + i*(FH + kFontLineDist) + (FH/2) + kFontLineDist;
    }

    // draw test state, name
    set_color(&font_color);
    fill_text(state_pos_.x, state_pos_.y, tests_[focus_case_id]->state_str(), 0);
    fill_text(name_pos_.x, name_pos_.y, tests_[focus_case_id]->name(), 0);

    // split result lines
    std::vector<std::string> lines(kResultLines);
    std::string result = tests_[focus_case_id]->result();
    split_string(&lines[0], kResultLines, result, '\n', kResultLineMaxChars);

//    lines.push_back(format_string("last frame cost %02d", get_last_frame_cost()/1000)); // show frame cost

    // draw result lines.
    for (int i = 0; i < kResultLines; i++) {
        fill_text(result_pos_[i].x, result_pos_[i].y, lines[i].c_str(), 0);
    }

    // draw QR code background
    set_color(&qr_bg_color); // background
    fill_rect(&qr_bg_rect_);

    // draw QR code
    set_color(&qr_fg_color); // foreground
    for (int i = 0; i < qr_size; i++) {
        for (int j = 0; j < qr_size; j++) {
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
        double a = kFreezeAngle/2 + (i + 0.5) * (2*M_PI - kFreezeAngle)/kCases;
        c[i].x = x0 - case_center_radius * sin(a);
        c[i].y = y0 + case_center_radius * cos(a);

        rotate_with(&t[i].a, &t0.a, &center, -a);
        rotate_with(&t[i].b, &t0.b, &center, -a);
        rotate_with(&t[i].c, &t0.c, &center, -a);

        case_colors[i] = &case_color;
    }

    qrdata_ = QrData::Builder::instance()
            .set_sn1(get_serial())
            .set_version(kVersion)
            .build();

    set_alarm(1);

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
    tests_[count++] = new DlpAgeTest(this, "DLP age test");
    tests_[count++] = new ClearFlash(this, "Clear flash");
    tests_[count++] = new FtmExit(this, "FTM exit");
    tests_[count++] = new PowerOff(this, "Power off");
    for (int i = count; i < kCases; i++) {
        tests_[i] = new EmptyTest(this, "");
        case_colors[i] = &empty_color;
    }

    load_results();
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
    fill_circle_ex(&border_color, x0, y0, border_radius);

    // gap
    fill_circle_ex(&gap_color, x0, y0, gap_radius);

    // main
    fill_circle_ex(&main_color, x0, y0, main_radius);

    // QR code, test name/state/result
    draw_main();

    // cases
    const point_t* c = case_centers;
    for (int i = 0; i < kCases; i++) {
        const color_t* color = case_colors[i];
        switch (tests_[i]->state()) {
            case UiTest::TS_PASSED:
                color = &pass_color;
                break;
            case UiTest::TS_FAILED:
                color = &fail_color;
                break;
            case UiTest::TS_TESTING:
                color = &testing_color;
                break;
            default:
                break;
        }
        fill_circle_ex(color, c[i].x, c[i].y, case_radius);

        char text[64];
        snprintf(text, sizeof(text), "%d", i+1);
        set_color(&font_color);
        fill_text(c[i].x, c[i].y, text, 1);
    }

    // arrow
    fill_triangle_ex(&arrow_color, &arrow_places[focus_case_id]);

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
    XLOGI("switch to test %d\n", focus_case_id);
    UiBase::SetCurrentUI(tests_[focus_case_id]);
}

void UiMain::OnAlarm()
{
    static Timer timer;
    XLOGI("alarm %.3f", timer.elapsed());
    set_alarm(10);
}

void UiMain::OnEnter()
{
    XLOGI("enter %p", this);
    save_results();
}

void UiMain::OnLeave()
{
    XLOGI("leave %p", this);
}

void UiMain::save_results()
{
#if 1
    for (std::string::size_type pos = 0, last = 0;
         pos != std::string::npos;) {
        last = pos + 1;
        pos = kResultDirectory.find('/', last);
        std::string dir = kResultDirectory.substr(0, pos);
        if (!file_exists(dir.c_str())) {
            XLOGD("mkdir %s", dir.c_str());
            if (mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
                XLOGE("mkdir %s, failed: %s", dir.c_str(), strerror(errno));
            }
        }
    }
#else
    if (!file_exists(kResultDirectory.c_str())) {
        std::string out;
        run_command("mkdir -p " + kResultDirectory, &out);
        XLOGI("mkdir -p %s", kResultDirectory.c_str());
    }
#endif

    for (int i = 0; i < kCases; i++) {
        if (tests_[i]->state() != UiTest::TS_INIT) {
            std::string filename, content;
            content += format_string("name: %s\n", tests_[i]->name());
            content += format_string("state: %d %s\n", tests_[i]->state(), tests_[i]->state_str());
            content += format_string("result: %s", tests_[i]->result().c_str());
            filename = format_string("%s/%d.txt", kResultDirectory.c_str(), i);
            if (write_file(filename.c_str(), content) < 0) {
                XLOGE("write %s failed, %s", filename.c_str(), strerror(errno));
            }
            XLOGI("saved %s, content: `%s`", filename.c_str(), content.c_str());
        }
    }
}

void UiMain::load_results()
{
    if (!file_exists(kResultDirectory.c_str())) {
        return;
    }

    for (int i = 0; i < kCases; i++) {
        std::string filename, content;
        filename = format_string("%s/%d.txt", kResultDirectory.c_str(), i);
        if (!file_exists(filename.c_str())
            || read_file(filename.c_str(), &content) < 0) {
            XLOGE("load test %d failed", i);
            continue;
        }

        auto pos = content.find("state: ");
        if (pos != std::string::npos) {
            int state = UiTest::TS_INIT;
            std::stringstream ls(content.substr(pos + sizeof("state: ")-1));
            ls >> state;
            tests_[i]->state(state);
            XLOGI("load test %d state: %s", i, tests_[i]->state_str());
        }

        pos = content.find("result: ");
        if (pos != std::string::npos) {
            tests_[i]->result(content.substr(pos + sizeof("result: ")-1));
            XLOGI("load test %d result: `%s`", i, tests_[i]->result().c_str());
        }
    }
}

void UiMain::clear_results()
{
    if (file_exists(kResultDirectory.c_str())) {
        std::string out;
        run_command("rm -rf " + kResultDirectory, &out);
    }
    load_results();
}
