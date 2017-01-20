//
// Created by dev on 16-12-3.
//

#include <unistd.h>
#include <dirent.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <map>

#include "utils.h"
#include "minui/minui.h"
#include "log.h"

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


static inline double dc(double r, double y)
{
    double x = sqrt(r*r - y*y);
    return ceil(x) - x;
}

static inline void draw_pixel(int x, int y)
{
    gr_fill(x, y, x+1, y+1);
}

// use Xiaolin Wu's Algoritm for anti-aliasing.
// https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm
void draw_circle(const color_t *c, int x0, int y0, int r)
{
    if (r <= 0) return;

    double d = 0.0;
    int y = 0, x = r;
    while (x > y) {
        double dx = dc(r, y);
        if (dx < d) x--;

        // Octant labelling
        //
        //  \ 5 | 6 /
        //   \  |  /
        //  4 \ | / 7
        //     \|/
        //------+------ +x
        //     /|\
        //  3 / | \ 0
        //   /  |  \
        //  / 2 | 1 \
        //      +y

        // draw outter side
        gr_color(c->r, c->g, c->b, c->a*(1 - dx));
        draw_pixel(x0 + x, y0 + y);  // 0
        draw_pixel(x0 - x, y0 + y);  // 3
        draw_pixel(x0 - x, y0 - y);  // 4
        draw_pixel(x0 + x, y0 - y);  // 7

        draw_pixel(x0 + y, y0 + x);  // 1
        draw_pixel(x0 - y, y0 + x);  // 2
        draw_pixel(x0 - y, y0 - x);  // 5
        draw_pixel(x0 + y, y0 - x);  // 6

        // draw inner side
        gr_color(c->r, c->g, c->b, c->a * dx);
        draw_pixel(x0 + (x-1), y0 + y);  // 0
        draw_pixel(x0 - (x-1), y0 + y);  // 3
        draw_pixel(x0 - (x-1), y0 - y);  // 4
        draw_pixel(x0 + (x-1), y0 - y);  // 7

        draw_pixel(x0 + y, y0 + (x-1));  // 1
        draw_pixel(x0 - y, y0 + (x-1));  // 2
        draw_pixel(x0 - y, y0 - (x-1));  // 5
        draw_pixel(x0 + y, y0 - (x-1));  // 6

        d = dx;
        y++;
    }
}

// use Xiaolin Wu's Algoritm for anti-aliasing.
// https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm
void fill_circle_ex(const color_t* c, int x0, int y0, int r)
{
    if (r <= 0) return;

    std::map<int, std::pair<int, int>> xrange;

    double d = 0.0;
    int y = 0, x = r;
    while (x > y) {
        double dx = dc(r, y);
        if (dx < d) x--;

        // Octant labelling
        //
        //  \ 5 | 6 /
        //   \  |  /
        //  4 \ | / 7
        //     \|/
        //------+------ +x
        //     /|\
        //  3 / | \ 0
        //   /  |  \
        //  / 2 | 1 \
        //      +y

        // draw outter side
        gr_color(c->r, c->g, c->b, c->a*(1 - dx));
        draw_pixel(x0 + x, y0 + y);  // 0
        draw_pixel(x0 - x, y0 + y);  // 3
        draw_pixel(x0 - x, y0 - y);  // 4
        draw_pixel(x0 + x, y0 - y);  // 7

        draw_pixel(x0 + y, y0 + x);  // 1
        draw_pixel(x0 - y, y0 + x);  // 2
        draw_pixel(x0 - y, y0 - x);  // 5
        draw_pixel(x0 + y, y0 - x);  // 6

        // calculate x rage
#define EXTEND_XRANGE(px0, px1, py0)                \
        if (xrange.find(py0) == xrange.end()) {     \
            xrange[py0] = std::make_pair(px0, px1); \
        } else {                                    \
            if ((px0) < xrange[py0].first) {        \
                xrange[py0].first = (px0);          \
            }                                       \
            if ((px1) > xrange[py0].second) {       \
                xrange[py0].second = (px1);         \
            }                                       \
        }

        EXTEND_XRANGE(x0 - x, x0 + x, y0 + y);  // 0 3
        EXTEND_XRANGE(x0 - x, x0 + x, y0 - y);  // 4 7
        EXTEND_XRANGE(x0 - y, x0 + y, y0 + x);  // 1 2
        EXTEND_XRANGE(x0 - y, x0 + y, y0 - x);  // 4 7

#undef EXTEND_XRANGE

        d = dx;
        y++;
    }

    // draw inner area
    gr_color(c->r, c->g, c->b, c->a);
    for (auto& p: xrange) {
        gr_fill(p.second.first,      p.first,
                p.second.second + 1, p.first + 1);
    }
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
    int W = gr_fb_width();
    int H = gr_fb_height();

    gr_surface pic = {0};
    int ret;

    ret = res_create_display_surface(name, &pic);
    if (ret < 0) {
        XLOGI("res_create_display_surface fail: %s", name);
        ret = res_create_alpha_surface(name, &pic);
        if (ret < 0) {
            XLOGI("res_create_alpha_surface fail: %s", name);
            return;
        }
    }

    int w = min(W, pic->width);
    int h = min(H, pic->height);
    int sx = max(0, (pic->width - W)/2);
    int sy = max(0, (pic->height - H)/2);
    int dx = max(0, (W - pic->width)/2);
    int dy = max(0, (H - pic->height)/2);
    gr_blit(pic, sx, sy, w, h, dx, dy);
    res_free_surface(pic);
}

static double fpart(double x)
{
    return x < 0 ? 1 - (x - floor(x)) : x - floor(x);
}

static double rfpart(double x)
{
    return round(1.0 - fpart(x));
}

void draw_line(const color_t *c, int x0, int y0, int x1, int y1)
{
    bool steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;
    double gradient = dy / (double)dx;

    double xend = round(x0);
    double yend = y0 + gradient * (xend - x0);
    double xgap = rfpart(x0 + 0.5);
    int xpx1 = xend;
    int ypx1 = (int) yend;

    // handle first endpoint
    if (steep) {
        gr_color(c->r, c->g, c->b, c->a * rfpart(yend) * xgap);
        draw_pixel(ypx1, xpx1);
        gr_color(c->r, c->g, c->b, c->a *  fpart(yend) * xgap);
        draw_pixel(ypx1+1, xpx1);
    } else {
        gr_color(c->r, c->g, c->b, c->a * rfpart(yend) * xgap);
        draw_pixel(xpx1, ypx1);
        gr_color(c->r, c->g, c->b, c->a *  fpart(yend) * xgap);
        draw_pixel(xpx1, ypx1+1);
    }
    double yinter = yend + gradient;  // first y-intersection

    // handle second endpoint
    xend = round(x1);
    yend = y1 + gradient * (xend - x1);
    xgap = fpart(x1 + 0.5);
    int xpx2 = xend;
    int ypx2 = (int) yend;
    if (steep) {
        gr_color(c->r, c->g, c->b, c->a * rfpart(yend) * xgap);
        draw_pixel(ypx2, xpx2);
        gr_color(c->r, c->g, c->b, c->a *  fpart(yend) * xgap);
        draw_pixel(ypx2+1, xpx2);
    } else {
        gr_color(c->r, c->g, c->b, c->a * rfpart(yend) * xgap);
        draw_pixel(xpx2, ypx2);
        gr_color(c->r, c->g, c->b, c->a *  fpart(yend) * xgap);
        draw_pixel(xpx2, ypx2+1);
    }

    if (steep) {
        for (int x = xpx1+1; x < xpx2; x++) {
            gr_color(c->r, c->g, c->b, c->a * rfpart(yinter));
            draw_pixel((int) yinter, x);
            gr_color(c->r, c->g, c->b, c->a *  fpart(yinter));
            draw_pixel((int) yinter + 1, x);
            yinter += gradient;
        }
    } else {
        for (int x = xpx1+1; x < xpx2; x++) {
            gr_color(c->r, c->g, c->b, c->a * rfpart(yinter));
            draw_pixel(x, (int) yinter);
            gr_color(c->r, c->g, c->b, c->a *  fpart(yinter));
            draw_pixel(x, (int) yinter + 1);
            yinter += gradient;
        }
    }
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

void fill_rect(const rect_t* rect)
{
    if (!rect) return;
    gr_fill(rect->x, rect->y, rect->x + rect->w, rect->y + rect->h);
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

int list_directory(std::vector<std::string>* result, const char* path)
{
    DIR* dp = opendir(path);
    if (!dp || !result) return -1;

    result->clear();
    struct dirent* de = NULL;
    while ((de = readdir(dp)) != NULL) {
        std::string name = de->d_name;
        result->push_back(name);
    }

    closedir(dp);
    return result->size();
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
    if (!out) return -1;
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

std::string format_string(const char* fmt, ...)
{
    char fixed[1024];

    va_list ap;
    va_start(ap, fmt);

    va_list ap_bakup;
    va_copy(ap_bakup, ap);
    int count = vsnprintf(fixed, sizeof(fixed), fmt, ap_bakup);
    va_end(ap_bakup);


    if (count < (int)sizeof(fixed)) {
        return fixed;
    }

    int length = count + 1;
    char* buff = (char*)malloc(length);
    va_copy(ap_bakup, ap);
    vsnprintf(buff, length, fmt, ap_bakup);
    va_end(ap_bakup);

    va_end(ap);

    std::string result(buff);
    free(buff);
    return result;
}

uint32_t checksum32(std::string s)
{
    uint32_t result = 0;
    if (s.size() % 4) {
        s.resize((s.size()/4 + 1)*4, '\0');
    }
    for (size_t i = 0; i < s.size(); i += 4) {
        result ^= (static_cast<uint32_t>(s[i]) << 24)
                + (static_cast<uint32_t>(s[i+1]) << 16)
                + (static_cast<uint32_t>(s[i+2]) << 8)
                + (static_cast<uint32_t>(s[i+3]));
    }
    return result;
}

uint16_t checksum16(std::string s)
{
    uint16_t result = 0;
    if (s.size() % 2) {
        s.resize((s.size()/2 + 1)*2, '\0');
    }
    for (size_t i = 0; i < s.size(); i += 2) {
        result ^= (static_cast<uint16_t>(s[i]) << 8)
                + (static_cast<uint16_t>(s[i+1]));
    }
    return result;
}

#if 0
static int __format_string_test__ = []() {
    puts("format_string test");
    puts(format_string("this is a %s", "test").c_str());
    return 0;
}();
#endif

