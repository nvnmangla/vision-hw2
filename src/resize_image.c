#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    int nn_x, nn_y;

    nn_x = (x < 0) ? round(x) + 1 : round(x);
    nn_y = (y < 0) ? round(y) + 1 : round(y);


    return get_pixel(im, nn_x, nn_y, c);
}

image nn_resize(image im, int w, int h)
{
    image im2 = make_image(w, h, im.c);
    float h_factor = h / im.h;
    float w_factor = w / im.w;
    for (int i = 0;i < h;i++) {
        for (int j = 0;j < w;j++) {

            float new_r = nn_interpolate(im, j / w_factor, i / h_factor, 0);
            float new_g = nn_interpolate(im, j / w_factor, i / h_factor, 1);
            float new_b = nn_interpolate(im, j / w_factor, i / h_factor, 2);

            set_pixel(im2, j, i, 0, new_r);
            set_pixel(im2, j, i, 1, new_g);
            set_pixel(im2, j, i, 2, new_b);

        }
    }

    return im2;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    float gap_y = y - floor(y);
    float gap_x = x - floor(x);

    float q1 = get_pixel(im, floor(x), ceil(y), c) * gap_y + get_pixel(im, floor(x), floor(y), c) * (1 - gap_y);
    float q2 = get_pixel(im, ceil(x), ceil(y), c) * gap_y + get_pixel(im, ceil(x), floor(y), c) * (1 - gap_y);
    float value = q1 * (1 - gap_x) + q2 * gap_x;
    return value;
}

image bilinear_resize(image im, int w, int h)

{
    // TODO
    image im2 = make_image(w, h, im.c);
    float h_factor = h / im.h;
    float w_factor = w / im.w;
    for (int i = 0;i < h;i++) {
        for (int j = 0;j < w;j++) {

            float new_r = bilinear_interpolate(im, j / w_factor, i / h_factor, 0);
            float new_g = bilinear_interpolate(im, j / w_factor, i / h_factor, 1);
            float new_b = bilinear_interpolate(im, j / w_factor, i / h_factor, 2);
            set_pixel(im2, j, i, 0, new_r);
            set_pixel(im2, j, i, 1, new_g);
            set_pixel(im2, j, i, 2, new_b);
        }
    }


    return im2;
}

