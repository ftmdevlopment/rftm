//
// Created by dev on 16-12-13.
//

#include <cstdint>
#include <cstdio>

static constexpr double a = -1e6;
// map value to (-1, 1)
static double normalize(int32_t v) {
    if (v < 0) {
        return -normalize(-v);
    }
    double x = v;
    return a/(x - a) + 1.0;
}

int main()
{
    for (int32_t i = 0; i < 2000; i++) {
        printf("%d\t%.6f\t%.6f\n", i, normalize(i), normalize(-i));
    }
    return 0;
}
