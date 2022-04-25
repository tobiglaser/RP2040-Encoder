#include <stdio.h>

#include "quadrature.h"

/*
    This example prints out the count every time it changes.

    If you are getting weird results,
    be aware there is no software debouncing in this implementation.

*/



constexpr int ENCODER1_DATA_PIN = 14;
constexpr int ENCODER1_CLK_PIN = 15;

int main() {
    stdio_init_all();

    Quadrature_encoder<ENCODER1_DATA_PIN, ENCODER1_CLK_PIN> encoder1 = Quadrature_encoder<ENCODER1_DATA_PIN, ENCODER1_CLK_PIN>();
    encoder1.begin(pull_direction::up, resolution::quarter);

    for (int countBefore = 0;;)
    {
        long count = encoder1.count();
        if (countBefore != count)
        {
            countBefore = count;
            printf("%d\n", count);
        }
    }
}