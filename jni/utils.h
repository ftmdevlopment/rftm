//
// Created by dev on 16-12-3.
//

#ifndef RFTM_UTILS_H
#define RFTM_UTILS_H

#include <vector>
#include <string>
#include <algorithm>
using std::max;
using std::min;

int read_file(const char* name, std::string* result, int expected = 0);
int write_file(const char* name, std::string content);
bool file_exists(const char* path);

int list_directory(std::vector<std::string>* result, const char* path);

int run_command(std::string cmd, std::string* out, bool err2out = true);
int split_string(std::string* out, int size, const std::string& in, const char sep = ' ', const int max = 0);
std::string trim_string(std::string input);

#ifdef __cplusplus
extern "C" {
#endif

struct point
{
    int x, y;
};
typedef struct point point_t;


struct triangle
{
    union {
        struct {
            point_t a, b, c;
        };
        point_t p[3];
    };
};
typedef struct triangle triangle_t;


struct color
{
    unsigned char r, g, b, a;
};
typedef struct color color_t;


void set_color(const color_t* c);
void fill_text(int x, int y, const char* text, int bold);
void fill_image(int x, int y, const char* name);
void fill_circle(int x0, int y0, int radius);
void fill_ring(int x0, int y0, int r1, int r2);
void fill_triangle(const triangle_t* t);

bool in_triangle(const triangle_t* t, int x, int y);
void rotate_with(point_t* result, const point_t* source, const point_t* center, double alpha);

#ifdef __cplusplus
}
#endif

#endif  // RFTM_UTILS_H
