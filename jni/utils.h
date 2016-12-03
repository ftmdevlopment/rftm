//
// Created by dev on 16-12-3.
//

#ifndef RFTM_UTILS_H
#define RFTM_UTILS_H

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
    point_t a, b, c;
};
typedef struct triangle triangle_t;


struct color
{
    unsigned char r, g, b, a;
};
typedef struct color color_t;


void set_color(const color_t* c);
void fill_text(int x, int y, const char* text);
void fill_image(int x, int y, const char* name);
void fill_circle(int x0, int y0, int radius);

void rotate_with(point_t* result, const point_t* source, const point_t* center, double alpha);

#ifdef __cplusplus
}
#endif

#endif  // RFTM_UTILS_H
