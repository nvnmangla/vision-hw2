
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853


void l1_normalize(image im)

{
    for (int c = 0;c < im.c;c++) {
        float sum = 0;
        for (int i = 0;i < im.h;i++) {
            for (int j = 0;j < im.w;j++) {
                sum += get_pixel(im, j, i, c);
            }

        }
        for (int i = 0;i < im.h;i++) {
            for (int j = 0;j < im.w;j++) {
                float v;
                v = get_pixel(im, j, i, c) / sum;
                set_pixel(im, j, i, c, v);
            }

        }


    }
}

image make_box_filter(int w)
{
    // TODO
    image filter = make_image(w, w, 1);
    for (int i = 0;i < w;i++) {
        for (int j = 0;j < w;j++) {
            set_pixel(filter, j, i, 0, 1);
        }
    }

    l1_normalize(filter);
    return filter;
}
float apply_filter(image im, image filter, int j, int i, int channel) {
    float sum = 0;

    int mid = (filter.w % 2 == 0) ? filter.w / 2 : (filter.w + 1) / 2;
    for (int a = 0;a < filter.h;++a) {
        for (int b = 0;b < filter.w;++b) {
            float value = get_pixel(im, b + j - mid + 1, a + i - mid + 1, channel) * get_pixel(filter, b, a, 0);
            sum = sum + value;
        }

    }
    return sum;
}

image convolve_image(image im, image filter, int preserve)
{
    float val;
    image blur;
    if (preserve == 1) {
        blur = make_image(im.w, im.h, im.c);
    }
    else {
        blur = make_image(im.w, im.h, 1);

    }
    for (int channel = 0;channel < blur.c;channel++) {
        for (int i = 0;i < blur.h;i++) {
            for (int j = 0;j < blur.w;j++) {
                val = apply_filter(im, filter, j, i, channel);
                set_pixel(blur, j, i, channel, val);
            }
        }
    }

    return blur;
}




image make_highpass_filter()
{
    // TODO
    image highpass = make_image(3, 3, 1);

    // Row 1
    set_pixel(highpass, 0, 0, 0, 0);
    set_pixel(highpass, 1, 0, 0, -1);
    set_pixel(highpass, 2, 0, 0, 0);
    // Row 2
    set_pixel(highpass, 0, 1, 0, -1);
    set_pixel(highpass, 1, 1, 0, 4);
    set_pixel(highpass, 2, 1, 0, -1);
    // Row 3
    set_pixel(highpass, 0, 2, 0, 0);
    set_pixel(highpass, 1, 2, 0, -1);
    set_pixel(highpass, 2, 2, 0, 0);


    return highpass;
}

image make_sharpen_filter()
{
    image sharp = make_image(3, 3, 1);

    // Row 1
    set_pixel(sharp, 0, 0, 0, 0);
    set_pixel(sharp, 1, 0, 0, -1);
    set_pixel(sharp, 2, 0, 0, 0);
    // Row 2
    set_pixel(sharp, 0, 1, 0, -1);
    set_pixel(sharp, 1, 1, 0, 5);
    set_pixel(sharp, 2, 1, 0, -1);
    // Row 3
    set_pixel(sharp, 0, 2, 0, 0);
    set_pixel(sharp, 1, 2, 0, -1);
    set_pixel(sharp, 2, 2, 0, 0);

    return sharp;
}

image make_emboss_filter()
{
    // TODO  
    image highpass = make_image(3, 3, 1);

    // Row 1
    set_pixel(highpass, 0, 0, 0, -2);
    set_pixel(highpass, 1, 0, 0, -1);
    set_pixel(highpass, 2, 0, 0, 0);
    // Row 2
    set_pixel(highpass, 0, 1, 0, -1);
    set_pixel(highpass, 1, 1, 0, 1);
    set_pixel(highpass, 2, 1, 0, 1);
    // Row 3
    set_pixel(highpass, 0, 2, 0, 0);
    set_pixel(highpass, 1, 2, 0, 1);
    set_pixel(highpass, 2, 2, 0, 2);


    return highpass;
    // return make_image(1,1,1);
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: TODO

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: TODO

image make_gaussian_filter(float sigma)

{
    int size = 6 * sigma;
    size = (size % 2 == 0) ? size + 1 : size;
    image filter = make_image(size, size, 1);
    int x, y;
    for (int i = 0;i < filter.h;i++) {
        for (int j = 0;j < filter.w;j++) {
            x = j - (filter.w + 1) / 2;
            y = i - (filter.h + 1) / 2;
            float value = (1 / (2 * 3.14 * sigma * sigma)) * exp(-(x * x + y * y) / (2 * sigma * sigma));
            set_pixel(filter, j, i, 0, value);
        }
    }


    return filter;
}

image add_image(image a, image b)
{
    assert(a.h == b.h && a.w == b.w && a.c == b.c);
    image new_image = make_image(a.w, a.h, a.c);
    float value;
    for (int c = 0;c < a.c;c++) {
        for (int i = 0;i < a.h;i++) {
            for (int j = 0;j < a.w;j++) {
                value = get_pixel(a, j, i, c) + get_pixel(b, j, i, c);
                set_pixel(new_image, j, i, c, value);

            }
        }
    }
    return new_image;
}

image sub_image(image a, image b)
{
    assert(a.h == b.h && a.w == b.w && a.c == b.c);
    image new_image = make_image(a.w, a.h, a.c);
    float value;
    for (int c = 0;c < a.c;c++) {
        for (int i = 0;i < a.h;i++) {
            for (int j = 0;j < a.w;j++) {
                value = get_pixel(a, j, i, c) - get_pixel(b, j, i, c);
                set_pixel(new_image, j, i, c, value);

            }
        }
    }
    return new_image;
}

image make_gx_filter()
{
    image g_x = make_image(3, 3, 1);
    set_pixel(g_x, 0, 0, 0, 1);
    set_pixel(g_x, 1, 0, 0, 0);
    set_pixel(g_x, 2, 0, 0, -1);
    // Row 2
    set_pixel(g_x, 0, 1, 0, 2);
    set_pixel(g_x, 1, 1, 0, 0);
    set_pixel(g_x, 2, 1, 0, -2);
    // Row 3
    set_pixel(g_x, 0, 2, 0, 1);
    set_pixel(g_x, 1, 2, 0, 0);
    set_pixel(g_x, 2, 2, 0, -1);
    return g_x;
}

image make_gy_filter()
{
    image g_y = make_image(3, 3, 1);
    set_pixel(g_y, 0, 0, 0, 1);
    set_pixel(g_y, 1, 0, 0, 2);
    set_pixel(g_y, 2, 0, 0, 1);
    // Row 2
    set_pixel(g_y, 0, 1, 0, 0);
    set_pixel(g_y, 1, 1, 0, 0);
    set_pixel(g_y, 2, 1, 0, 0);
    // Row 3
    set_pixel(g_y, 0, 2, 0, -1);
    set_pixel(g_y, 1, 2, 0, -2);
    set_pixel(g_y, 2, 2, 0, -1);
    return g_y;
}

float three_way_maximum(float a, float b, float c)
{
    return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
}

float three_way_minimum(float a, float b, float c)
{
    return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
}

void feature_normalize(image im)
{
    float min_r = get_pixel(im, 0, 0, 0);
    float min_g = get_pixel(im, 0, 0, 1);
    float min_b = get_pixel(im, 0, 0, 2);

    float max_r = get_pixel(im, 0, 0, 0);
    float max_g = get_pixel(im, 0, 0, 1);
    float max_b = get_pixel(im, 0, 0, 2);

    float r_value, g_value, b_value;
    for (int row = 0; row < im.h; row++) {
        for (int col = 0; col < im.w; col++) {
            r_value = get_pixel(im, col, row, 0);
            g_value = get_pixel(im, col, row, 1);
            b_value = get_pixel(im, col, row, 2);

            min_r = (r_value <= min_r) ? r_value : min_r;
            min_g = (g_value <= min_g) ? g_value : min_g;
            min_b = (b_value <= min_b) ? b_value : min_b;

            max_r = (r_value >= max_r) ? r_value : max_r;
            max_g = (g_value >= max_g) ? g_value : max_g;
            max_b = (b_value >= max_b) ? b_value : max_b;

        }
    }
    float min = three_way_minimum(min_r, min_b, min_g);
    float max = three_way_maximum(max_b, max_g, max_r);
    float value;
    for (int channel = 0;channel < im.c;channel++) {
        for (int row = 0; row < im.h; row++) {
            for (int col = 0; col < im.w; col++) {
                value = (max - min == 0) ? 0 : (get_pixel(im, col, row, channel) - min) / (max - min);
                set_pixel(im, col, row, channel, value);
            }
        }
    }
}

image* sobel_image(image im)
{
    image* sobel = calloc(2, sizeof(im));

    image g_x = make_gx_filter();
    image g_y = make_gy_filter();
    float val, angle;
    image g_x_image = convolve_image(im, g_x, 0);
    image g_y_image = convolve_image(im, g_y, 0);

    image mag = make_image(im.w, im.h, im.c);
    image ang = make_image(im.w, im.h, im.c);

    for (int c = 0; c < mag.c; c++) {
        for (int h = 0; h < mag.h; h++) {
            for (int w = 0; w < mag.w; w++) {
                val = sqrt(get_pixel(g_x_image, w, h, 0) * get_pixel(g_x_image, w, h, 0) + get_pixel(g_y_image, w, h, 0) * get_pixel(g_y_image, w, h, 0));
                angle = atan(get_pixel(g_y_image, w, h, 0) / get_pixel(g_x_image, w, h, 0));
                set_pixel(mag, w, h, c, val);
                set_pixel(ang, w, h, c, angle);

            }
        }
    }
    sobel[0] = mag;
    sobel[1] = ang;
    return sobel;
}

image colorize_sobel(image im)
{
    image g_x = make_gx_filter();
    image g_y = make_gy_filter();
    float val, angle;
    image g_x_image = convolve_image(im, g_x, 1);
    image g_y_image = convolve_image(im, g_y, 1);

    image mag = make_image(im.w, im.h, im.c);
    image ang = make_image(im.w, im.h, im.c);

    for (int c = 0; c < mag.c; c++) {
        for (int h = 0; h < mag.h; h++) {
            for (int w = 0; w < mag.w; w++) {
                val = sqrt(get_pixel(g_x_image, w, h, c) * get_pixel(g_x_image, w, h, c) + get_pixel(g_y_image, w, h, c) * get_pixel(g_y_image, w, h, c));
                angle = atan(get_pixel(g_y_image, w, h, c) / get_pixel(g_x_image, w, h, c));
                set_pixel(mag, w, h, c, val);
                set_pixel(ang, w, h, c, angle);

            }
        }
    }
    return mag;
}
