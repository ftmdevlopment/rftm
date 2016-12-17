//
// Created by dev on 16-12-7.
//

#include "triangle_test.h"

TirangleTest::TirangleTest(UiBase* main, const char* name)
    : UiTest(main, name)
{
    int W = gr_fb_width(), H = gr_fb_height();
    int x0 = W/2, y0 = H/2;
    radius_ = min(W/2, H/2)*2/3;
    for (int i = 0; i < 3; i++) {
        double a = i*(2*M_PI)/3;
        triangle_.p[i].x = x0 + radius_*sin(a);
        triangle_.p[i].y = y0 - radius_*cos(a);
    }
}


void TirangleTest::Draw()
{
    gr_color( 0xff, 0xff, 0xff, 0xff); gr_clear();

    gr_color( 0xff, 0x00, 0x00, 0xff);
    fill_triangle(&triangle_);

    gr_flip();
}

