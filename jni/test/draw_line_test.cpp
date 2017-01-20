//
// Created by xu on 17-1-20.
//

#include "draw_line_test.h"

void DrawLineTest::OnEnter()
{
    UiTest::OnEnter();


    int W = gr_fb_width();
    int H = gr_fb_height();
    int x0 = W/2, y0 = H/2;
    int R = std::min(x0, y0) - 20;

    point_t C = {W/2, H/2};
    point_t P0 = {W/2 + 10, H/2};
    point_t P1 = {W/2 + 110, H/2};

    XLOGI("center: %d %d", C.x, C.y);
    XLOGI("P0: %d %d", P0.x, P0.y);
    XLOGI("P1: %d %d", P1.x, P1.y);

    auto dist = [](point_t a, point_t b) {
        return sqrt(double(b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
    };

    for (int i = 0; i < N; i++) {
        double alpha = - i * 2*M_PI/N;
        rotate_with(&endpoints[i].first, &P0, &C, alpha);
        rotate_with(&endpoints[i].second, &P1, &C, alpha);

        printf("endpoints[%d] = {{%d, %d}, {%d, %d}}; \n", i,
               endpoints[i].first.x, endpoints[i].first.y,
               endpoints[i].second.x, endpoints[i].second.y);
        printf("// %.3f %.3f\n", alpha / M_PI * 180, dist(endpoints[i].first, endpoints[i].second));
    }
}


void DrawLineTest::Draw()
{
    color_t end_color = {0xff, 0xff, 0x00, 0xff};
    color_t pen_color = {0xff, 0xff, 0xff, 0xff};

    gr_color(0x00, 0x00, 0x00, 0xff);
    gr_clear();

    // draw lines
    for (int i = 0; i < N; i++) {
        auto line = endpoints[i];
        draw_line(&pen_color, line.first.x, line.first.y, line.second.x, line.second.y);
    }

    // draw end points
    for (int i = 0; i < N; i++) {
        set_color(&end_color);
        gr_fill(endpoints[i].first.x, endpoints[i].first.y,
                endpoints[i].first.x + 1, endpoints[i].first.y + 1);
        gr_fill(endpoints[i].second.x, endpoints[i].second.y,
                endpoints[i].second.x + 1, endpoints[i].second.y + 1);
    }
    gr_flip();
}

void DrawLineTest::RunTest()
{
    sleep(5);
}

