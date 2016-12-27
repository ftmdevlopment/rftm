//
// Created by dev on 16-12-6.
//

#include <stdio.h>
#include "draw_test.h"

DrawTest::DrawTest(UiBase* main, const char* name)
        : UiTest(main, name)
{
}

void DrawTest::Draw()
{
    int W = gr_fb_width();
    int H = gr_fb_height();
    int xa =   0, ya = 0;
    int xb = W/2, yb = 0;
    int xc = W/2, yc = H/2;

    gr_color( 255, 255, 255, 255); gr_clear(); // white bg
    gr_color( 255,   0,   0, 255); gr_fill( xa, ya, xa+W/2, ya+H/2);
    gr_color(   0, 255,   0, 255); gr_fill( xb, yb, xb+W/2, yb+H/2);
    gr_color(   0,   0, 255, 255); gr_fill( xc, yc, xc+W/2, yc+H/2);
    gr_color(   0,   0,   0, 255);
    fill_text(W/2, H/2, name(), 1);

    gr_flip();
}

void DrawTest::RunTest() {
    sleep(5);
}
