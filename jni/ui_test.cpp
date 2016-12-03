//
// Created by dev on 16-12-3.
//

#include <unistd.h>
#include <stdio.h>

#include "ui_test.h"
#include "minui/minui.h"

#define msleep(n) usleep((n) * 1000)

void UiTest::Draw()
{
    int i, W, H;
    int xa, xb, xc;
    int ya, yb, yc;

    puts(__func__);
    W = gr_fb_width();
    H = gr_fb_height();

    // layout:
    //      A    B
    //      C    D
    xa = xc = 0; xb = W/2;
    ya = yb = 0; yc = H/2;
    for (i = 255; i >= 0; i--) {
        gr_color( 255, 255, 255, 255); gr_clear(); // white bg
        gr_color( 255,   0,   0,   i); gr_fill( xa, ya, xa+W/2, ya+H/2);
        gr_color(   0, 255,   0,   i); gr_fill( xb, yb, xb+W/2, yb+H/2);
        gr_color(   0,   0, 255,   i); gr_fill( xc, yc, xc+W/2, yc+H/2);
        gr_flip();
    }
}

void UiTest::OnKey(int code, int value)
{
    printf("%s %d %d\n", __func__, code, value);
}

void UiTest::OnLeftTouch(int code, int value)
{
    printf("%s %d %d\n", __func__, code, value);
}

void UiTest::OnRightTouch(int code, int value)
{
    printf("%s %d %d\n", __func__, code, value);
}
