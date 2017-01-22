//
// Created by dev on 16-12-7.
//

#include "triangle_test.h"

#include <math.h>

TriangleTest::TriangleTest(UiBase* main, const char* name)
    : UiTest(main, name)
{
    int W = gr_fb_width(), H = gr_fb_height();
    int x0 = W/2, y0 = H/2;

    radius_ = min(W/2, H/2) * 3/5;

    for (int i = 0; i < 3; i++) {
        double a = i*(2*M_PI)/3;
        double r = edge_length_ / sqrt(3.0);
        T0.p[i].x = x0 + r * sin(a);
        T0.p[i].y = y0 - r * cos(a);
    }

    triangle_t T1 = T0;
    for (int i = 0; i < 3; i++) {
        T1.p[i].y = T0.p[i].y + radius_;
    }

    point_t C = {W/2, H/2};
    for (int i = 0; i < N; i++) {
        double a = - i * 2*M_PI/N;
        rotate_with(&triangles_[i].a, &T1.a, &C, a);
        rotate_with(&triangles_[i].b, &T1.b, &C, a);
        rotate_with(&triangles_[i].c, &T1.c, &C, a);
    }
}


void TriangleTest::Draw()
{
    gr_color( 0x00, 0x00, 0x00, 0xff); gr_clear();

    gr_color( 0xff, 0x00, 0x00, 0xff);
    fill_triangle(&T0);

    color_t c = {0xff, 0xff, 0x00, 0xff};
    for (int i = 0; i < N; i++) {
        fill_triangle_ex(&c, &triangles_[i]);
    }

    gr_flip();
}

void TriangleTest::RunTest()
{
    sleep(5);
}

void TriangleTest::OnEnter()
{
    UiTest::OnEnter();

    origin_fps_ = GetExpectedFPS();
    SetExpectedFPS(1);
}

void TriangleTest::OnLeave()
{
    UiTest::OnLeave();

    SetExpectedFPS(origin_fps_);
}

