//
// Created by dev on 16-12-3.
//

#include <math.h>
#include <string.h>

#include "utils.h"
#include "minui/minui.h"

#include <memory>

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
}

void set_color(const color_t* c)
{
    gr_color(c->r, c->g, c->b, c->a);
}


void fill_circle(int x0, int y0, int radius)
{
#if 0
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
#else
    fill_ring(x0, y0, 0, radius);
#endif
}

void fill_ring(int x0, int y0, int r1, int r2)
{
    if (r1 > r2) return;
    int W = gr_fb_width();
    int H = gr_fb_height();

    int xmin = max(0, x0 - r2);
    int ymin = max(0, y0 - r2);
    int xmax = min(W, x0 + r2 + 1);
    int ymax = min(H, y0 + r2 + 1);

    int s1 = r1*r1, s2 = r2*r2;
    for (int x = xmin; x <= xmax; x++) {
        for (int y = ymin; y <= ymax; y++) {
            int dx = x - x0, dy = y - y0;
            int s = dx*dx + dy*dy;
            if (s1 <= s && s <= s2) {
                gr_fill(x, y, x+1, y+1);
            }
        }
    }
}

void fill_text(int x, int y, const char* text, int bold)
{
    int fw, fh, tw, th;
    gr_font_size(&fw, &fh);
    tw = gr_measure(text); th = fh;
    gr_text(x - tw/2, y - th/2, text, bold);
}

// TODO
void fill_image(int x, int y, const char* name)
{
}

// assume that you stand on P1(your head point to z+), face to P2,
// this function determines weather P3 on your left side or not.
static bool onleft(const point_t* p1, const point_t* p2, const point_t* p3)
{
    point_t v1 = {p2->x - p1->x, p2->y - p1->y};
    point_t v2 = {p3->x - p1->x, p3->y - p1->y};
    return (v1.x * v2.y - v1.y * v2.x) > 0;
}

bool in_triangle(const triangle_t* t, int x, int y)
{
    if (!t) return false;

    point_t p = {x, y};
    bool b1 = onleft(&t->a, &t->b, &p);
    bool b2 = onleft(&t->b, &t->c, &p);
    bool b3 = onleft(&t->c, &t->a, &p);

    return ((b1 == b2) && (b2 == b3));
}

void fill_triangle(const triangle_t* t)
{
    int x, y;
    int xmin, xmax, ymin, ymax;
    xmin = min(t->a.x, min(t->b.x, t->c.x));
    xmax = max(t->a.x, max(t->b.x, t->c.x));
    ymin = min(t->a.y, min(t->b.y, t->c.y));
    ymax = max(t->a.y, max(t->b.y, t->c.y));

    for (x = xmin; x <= xmax; x++) {
        for (y = ymin; y <= ymax; y++) {
            if (in_triangle(t, x, y)) {
                gr_fill(x, y, x+1, y+1);
            }
        }
    }
}

int read_file(const char* name, std::string* result)
{
    long size = 0;
#if 0
    std::shared_ptr<FILE> sp(fopen(name, "rb"), fclose);
    if (!result || !stream) return -1;

    fseek(stream.get(), 0, SEEK_END);
    size = ftell(stream.get());
    fseek(stream.get(), 0, SEEK_SET);

    std::shared_ptr<char> buffer((char*)malloc(size), free);
    int count = fread(buffer.get(), 1, size, stream.get());
    *result = buffer.get();
#else
    FILE* stream = fopen(name, "rb");
    if (!result || !stream) return -1;

    fseek(stream, 0, SEEK_END);
    size = ftell(stream);
    fseek(stream, 0, SEEK_SET);

    char* buffer = (char*)malloc(size);
    int count = fread(buffer, 1, size, stream);

    *result = buffer;
    free(buffer);
    fclose(stream);
#endif
    return count;
}

int write_file(const char* name, std::string content)
{
#if 0
    std::shared_ptr<FILE> stream(fopen(name, "wb+"), fclose);
    if (!stream) return -1;
    int count = fwrite(&content[0], 1, content.size()+1, stream.get());
#else
    FILE* stream = fopen(name, "wb+");
    if (!stream) return -1;
    int count = fwrite(&content[0], 1, content.size()+1, stream);
    fclose(stream);
#endif
    return count;
}

static int read_write_test()
{
    std::string content, actual;
    for (int i = 0; i < 1000; i++) {
        content += "0123456789ABCDEF"[i % 16];
    }
    write_file("/system/test.txt", content);
    read_file("/system/test.txt", &actual);
    printf("write: %lu %s\n", content.size(), content.c_str());
    printf("read: %lu %s\n", actual.size(), actual.c_str());
    return 0;
}

//static int __test_stub__ = read_write_test();

int run_command(const char* cmd, std::string* out)
{
    if (!out) return -1;

    FILE* p = popen(cmd, "r");
    if (!p) perror("popen");

    out->resize(0);
    char buffer[1024] = {0};
    while (fgets(buffer, sizeof(buffer), p)) {
        *out += buffer;
    }

    fclose(p);
    return out->length();
}

