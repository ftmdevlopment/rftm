//
// Created by dev on 16-12-3.
//

#include <unistd.h>
#include <math.h>
#include <ctype.h>
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

int read_file(const char* name, std::string* result, int expected)
{
    long size = 0;

    FILE* stream = fopen(name, "rb");
    if (!result || !stream) return -1;

    if (expected) {
        size = expected;
    } else {
        fseek(stream, 0, SEEK_END);
        size = ftell(stream);
        fseek(stream, 0, SEEK_SET);
    }

    char* buffer = (char*)malloc(size);
    int count = fread(buffer, 1, size, stream);

    *result = buffer;
    free(buffer);
    fclose(stream);
    return count;
}

int write_file(const char* name, std::string content)
{
    FILE* stream = fopen(name, "wb+");
    if (!stream) return -1;
    int count = fwrite(&content[0], 1, content.size()+1, stream);
    fclose(stream);
    return count;
}

bool file_exists(const char* path)
{
    return access(path, F_OK) == 0;
}

#if 0
static int __read_write_test__ = []() {
    std::string content, actual;
    for (int i = 0; i < 1000; i++) {
        content += "0123456789ABCDEF"[i % 16];
    }
    write_file("/system/test.txt", content);
    read_file("/system/test.txt", &actual);
    printf("write: %lu %s\n", content.size(), content.c_str());
    printf("read: %lu %s\n", actual.size(), actual.c_str());
    return 0;
}();
#endif

int run_command(std::string cmd, std::string* out, bool err2out)
{
    if (!out) return -1;

    if (err2out) {
        cmd += " 2>&1";
    }
    FILE* p = popen(cmd.c_str(), "r");
    if (!p) perror("popen");

    out->clear();
    char buffer[1024] = {0};
    while (fgets(buffer, sizeof(buffer), p)) {
        *out += buffer;
    }

    pclose(p);
    return out->length();
}

int split_string(std::string* out, int size, const std::string& in, const char sep, const int max)
{
    std::string::size_type last = 0, next = 0;
    for (int i = 0; i < size; i++) {
        next = in.find(sep, last);
        if (next == std::string::npos) {
            next = in.size();
        }

        bool broken = false;
        if (max > 0 && next > last + max) {
            next = last + max;
            broken = true;  // reach line limit
        }

        out[i] = in.substr(last, next - last);
        if (next >= in.size()) {
            return i;
        }

        last = next;
        if (!broken) ++last; // skip separator
    }
    return size;
}

#if 0
static int __split_string_test__ = []() {
    std::string input = "UiTest::RunTestUiTest::RunTest\ndone\n3\n4\n5\n6\n7";
    std::string output[5];
    split_string(output, 5, input, '\n', 20);
    for (int i = 0; i < 5; i++) {
        printf("output[%d]: `%s`\n", i, output[i].c_str());
    }
    split_string(output, 5, input, '\n');
    for (int i = 0; i < 5; i++) {
        printf("output[%d]: `%s`\n", i, output[i].c_str());
    }
    return 0;
}();
#endif

std::string trim_string(std::string input)
{
    if (input.empty()) return input;

    std::string::size_type begin = 0, end = input.size();
    while (isspace(input[begin])) begin++;
    while (isspace(input[end-1])) end--;
    if (begin < end) {
        return input.substr(begin, end - begin);
    }
    return "";
}

#if 0
static int __trim_string_test__ = []() {
    std::string s = "\t\n \ra b c     ";
    printf("begin trim: `%s`, after: `%s`\n", s.c_str(), trim_string(s).c_str());
    return 0;
}();
#endif

