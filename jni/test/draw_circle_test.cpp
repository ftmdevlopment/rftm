//
// Created by xu on 17-1-19.
//

#include "draw_circle_test.h"

void DrawCircleTest::Draw()
{
    color_t line_color = {0xff, 0xff, 0xff, 0x88};
    color_t fill_color = {0xff, 0xa0, 0x00, 0x77};

    gr_color(0x00, 0x00, 0x00, 0xff);  // back ground
    gr_clear();

    int W = gr_fb_width();
    int H = gr_fb_height();
    int x0 = W/2, y0 = H/2;
    int R = std::min(x0, y0);

    int N = 9, lastR = 0;
    for (int i = 0, r = R/N; i < N-1; i++, r += R/N) {
        draw_circle(&line_color, x0, y0, r);
        lastR = r;
    }

    fill_circle_ex(&fill_color, x0, y0, lastR - R/N);

    gr_flip();
}

void DrawCircleTest::RunTest()
{
    sleep(5);
}
