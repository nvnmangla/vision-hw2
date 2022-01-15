
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    // TODO Fill this in
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x > im.w) x = im.w - 1;
    if (y > im.h) y = im.h - 1;
    int location = c * (im.h * im.w) + y * im.w + x;
    return im.data[location];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    if (x < 0)
        return;
    if (y < 0)
        return;
    if (x > im.w)
        return;
    if (y > im.h)
        return;
    
    int location = c * im.h * im.w + y * im.w + x;
    float* value = &im.data[location];
    *value = v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    for (int i = 0;i < im.h * im.w * im.c;i++) {
        float* v = &copy.data[i];
        *v = im.data[i];

    }

    // TODO Fill this in
    return copy;
}

image rgb_to_grayscale(image im)
{
    int r_location;
    int g_location;
    int b_location;
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    for (int i = 0;i < im.h;i++) {
        for (int j = 0;j < im.w;j++) {

            r_location = i * im.w + j;
            g_location = i * im.w + j + (im.h * im.w);
            b_location = i * im.w + j + 2 * (im.h * im.w);

            float v = 0.299 * im.data[r_location] + 0.587 * im.data[g_location] + 0.114 * im.data[b_location];
            int location = i * im.w + j;
            float* value2 = &gray.data[location];
            *value2 = v;
        }
    }

    return gray;
}

void shift_image(image im, int c, float v)
{
    for (int i = 0;i < im.h;i++) {
        for (int j = 0;j < im.w;j++) {
            int location = c * (im.h * im.w) + i * im.w + j;
            float* value = &im.data[location];
            float check = *value + v;
            if (check > 1) { *value = 1; }
            else if (check < 0) { *value = 0; }
            else { *value += v; }
        }
    }

    // TODO Fill this in
}

void clamp_image(image im)
{
    for (int c = 0;im.c < 3;c++) {
        for (int i = 0;i < im.h;i++) {
            for (int j = 0;j < im.w;j++) {
                int location = c * (im.h * im.w) + i * im.w + j;
                float* value = &im.data[location];
                if (*value < 1 && *value > 0) { *value = *value; }
                else if (*value < 0) { *value = 0; }
                else if (*value >1 ) { *value = 1; }

            }
        }
    }
}




// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
}

void rgb_to_hsv(image im) {
    float saturation = 0;
    int hue = 0;
    for (int i = 0;i < im.h;i++) {
        for (int j = 0;j < im.w;j++) {
            float r = get_pixel(im, j, i, 0);
            float g = get_pixel(im, j, i, 1);
            float b = get_pixel(im, j, i, 2);
            float val = three_way_max(r, g, b);
            float min = three_way_min(r, g, b);
            float diff = val - min;
            if (val != 0) { saturation = diff / val; }
            else { saturation = 0; }
            if (saturation == 0) { hue = 0; }
            else if (val == r) { hue = (g - b) / saturation; }
            else if (val == g) { hue = (b - r) / saturation + 2; }
            else { hue = (r - g) / saturation + 4; }
            // hue = hue % 6;
            float hue_final = hue / 6;
            if (hue < 0) { hue_final = hue / 6 + 1; }

            // replacing channels 
            set_pixel(im, j, i, 0, hue_final);
            set_pixel(im, j, i, 1, saturation);
            set_pixel(im, j, i, 2, val);
        }

    }
}
void hsv_to_rgb(image im) {
    // TODO Fill this in
    float r = 0;
    float b = 0;
    float g = 0;

    for (int i = 0;i < im.h;++i) {
        for (int j = 0;j < im.w;++j) {
            float hue = get_pixel(im, j, i, 0);
            float sat = get_pixel(im, j, i, 1);
            float val = get_pixel(im, j, i, 2);
            float c = sat * val;
            int hue2 = hue;
            float const x = c * (1 - abs(hue2 % 2 - 1));
            hue2 = hue2 % 6;
            if (0 <= hue2 && hue2 < 1) {
                r = c;
                g = x;
                b = 0;
            }
            else if (1 <= hue2 && hue2 < 2) {
                r = x;
                g = c;
                b = 0;
            }
            else if (2 <= hue2 && hue2 < 3) {
                g = c;
                b = x;
                r = 0;
            }
            else if (3 <= hue2 && hue2 < 4) {
                g = x;
                b = c;
                r = 0;
            }
            else if (4 <= hue2 && hue2 < 5) {
                r = x;
                b = c;
                g = 0;
            }
            else if (5 <= hue2 && hue2 < 6) {
                r = c;
                b = x;
                g = 0;
            }
            else {
                r = 0;
                b = 0;
                g = 0;
            }
            float m = val - c;
            r += m;
            g += m;
            b += m;
            set_pixel(im, j, i, 0, r);
            set_pixel(im, j, i, 1, g);
            set_pixel(im, j, i, 2, b);
        }

    }
}




