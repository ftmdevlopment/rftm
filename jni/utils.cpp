//
// Created by dev on 16-12-3.
//

#include <math.h>
#include <string.h>

#include "utils.h"
#include "minui/minui.h"

void rotate_with(point_t* result, const point_t* source, const point_t* center, double alpha)
{
    double x1_, y1_, x2_, y2_;
    if (result == NULL || center == NULL || source == NULL) return;
    if (memcmp(center, source, sizeof(*source)) == 0) return;

    x1_ =  source->x - center->x;
    y1_ = -source->y + center->y;

    x2_ = x1_*cos(alpha) - y1_*sin(alpha);
    y2_ = x1_*sin(alpha) + y1_*cos(alpha);

    result->x = center->x + x2_;
    result->y = center->y - y2_;

    gr_info("(%d, %d) --{(%d, %d), %g}--> (%d, %d)",
           source->x, source->y, center->x, center->y, alpha, result->x, result->y);
}

void set_color(const color_t* c)
{
    gr_color(c->r, c->g, c->b, c->a);
}


void fill_circle(int x0, int y0, int radius)
{
    int x, y, r2, W, H, xmin, xmax, ymin, ymax;
    W = gr_fb_width();
    H = gr_fb_height();
    xmin = max(0, x0 - radius);
    ymin = max(0, y0 - radius);
    xmax = min(W, x0 + radius + 1);
    ymax = min(H, y0 + radius + 1);
    r2 = radius * radius;
    for (x = xmin; x <= xmax; x++) {
        for (y = ymin; y <= ymax; y++) {
            int dx = x - x0, dy = y - y0;
            if (dx * dx + dy * dy <= r2) {
                gr_fill(x, y, x+1, y+1);
            }
        }
    }
}

void fill_text(int x, int y, const char* text)
{
    int fw, fh, tw, th;
    gr_font_size(&fw, &fh);
    tw = gr_measure(text); th = fh;
    gr_text(x - tw/2, y - th/2, text, 0);
}

// TODO
void fill_image(int x, int y, const char* name)
{
}


bool in_triangle(const triangle_t* t, point_t* p)
{
    return false;
}

void fill_triangle(const triangle_t* t)
{
    int x, y;
    int xmin, xmax, ymin, ymax;
    xmin = fmin(t->a.x, fmin(t->b.x, t->c.x));
    xmax = fmax(t->a.x, fmax(t->b.x, t->c.x));
    ymin = fmin(t->a.y, fmin(t->b.y, t->c.y));
    ymax = fmax(t->a.y, fmax(t->b.y, t->c.y));

    for (x = xmin; x <= xmax; x++) {
        for (y = ymin; y <= ymax; y++) {
            // TODO
        }
    }
}
