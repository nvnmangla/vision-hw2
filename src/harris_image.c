#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#include "matrix.h"
#include <time.h>

// Frees an array of descriptors.
// descriptor *d: the array.
// int n: number of elements in array.
void free_descriptors(descriptor* d, int n)
{
    int i;
    for (i = 0; i < n; ++i) {
        free(d[i].data);
    }
    free(d);
}

// Create a feature descriptor for an index in an image.
// image im: source image.
// int i: index in image for the pixel we want to describe.
// returns: descriptor for that index.

descriptor describe_index(image im, int i)
{
    int w = 5;
    descriptor d;
    d.p.x = i % im.w;
    d.p.y = i / im.w;
    d.data = calloc(w * w * im.c, sizeof(float));
    d.n = w * w * im.c;
    int c, dx, dy;
    int count = 0;

    // If you want you can experiment with other descriptors
    // This subtracts the central value from neighbors
    // to compensate some for exposure/lighting changes.

    for (c = 0; c < im.c; ++c) {
        float cval = im.data[c * im.w * im.h + i];
        for (dx = -w / 2; dx < (w + 1) / 2; ++dx) {
            for (dy = -w / 2; dy < (w + 1) / 2; ++dy) {
                float val = get_pixel(im, i % im.w + dx, i / im.w + dy, c);
                d.data[count++] = cval - val;
            }
        }
    }
    return d;
}

// Marks the spot of a point in an image.
// image im: image to mark.
// ponit p: spot to mark in the image.
void mark_spot(image im, point p)
{
    int x = p.x;
    int y = p.y;
    int i;
    for (i = -9; i < 10; ++i) {
        set_pixel(im, x + i, y, 0, 1);
        set_pixel(im, x, y + i, 0, 1);
        set_pixel(im, x + i, y, 1, 0);
        set_pixel(im, x, y + i, 1, 0);
        set_pixel(im, x + i, y, 2, 1);
        set_pixel(im, x, y + i, 2, 1);
    }
}

// Marks corners denoted by an array of descriptors.
// image im: image to mark.
// descriptor *d: corners in the image.
// int n: number of descriptors to mark.

void mark_corners(image im, descriptor* d, int n)
{
    int i;
    for (i = 0; i < n; ++i) {
        mark_spot(im, d[i].p);
    }
}

// Creates a 1d Gaussian filter.
// float sigma: standard deviation of Gaussian.
// returns: single row image of the filter.
image make_1d_gaussian(float sigma)
{
    // TODO: optional, make separable 1d Gaussian.
    return make_image(1, 1, 1);
}

// Smooths an image using separable Gaussian filter.
// image im: image to smooth.
// float sigma: std dev. for Gaussian.
// returns: smoothed image.
image smooth_image(image im, float sigma)
{
    if (1) {
        image g = make_gaussian_filter(sigma);
        image s = convolve_image(im, g, 1);
        free_image(g);
        return s;
    }
    else {
        // TODO: optional, use two convolutions with 1d gaussian filter.
        // If you implement, disable the above if check.
        return copy_image(im);
    }
}

// Calculate the structure matrix of an image.
// image im: the input image.
// float sigma: std dev. to use for weighted sum.
// returns: structure matrix. 1st channel is Ix^2, 2nd channel is Iy^2,
//          third channel is IxIy.
image structure_matrix(image im, float sigma)
{
    printf("Making structure! All well");
    image S = make_image(im.w, im.h, im.c);
    image Gx = make_image(3, 3, 1);
    image Gy = make_image(3, 3, 1);
    for (int i = 0;i < Gx.h * Gx.w * Gx.c;i++) {
        float* v_x = &Gx.data[i];
        float* v_y = &Gy.data[i];

        *v_x = 0;
        *v_y = 0;
    }
    set_pixel(Gx, 1, 0, 0, -1);
    set_pixel(Gx, 1, 2, 0, 1);
    set_pixel(Gy, 0, 1, 0, -1);
    set_pixel(Gy, 2, 1, 0, 1);

    // Convolving Gx and Gy with the Image to get derivatives.
    image Ix = convolve_image(im, Gx, 0);
    image Iy = convolve_image(im, Gy, 0);
    image IxIx = make_image(im.w, im.h, 1);
    image IxIy = make_image(im.w, im.h, 1);
    image IyIy = make_image(im.w, im.h, 1);
    for (int i = 0;i < im.w * im.h * im.c;i++) {
        float* v_x = &Ix.data[i];
        float* v_y = &Iy.data[i];
        float* v_xx = &IxIx.data[i];
        float* v_yy = &IyIy.data[i];
        float* v_xy = &IxIy.data[i];

        *v_xx = *v_x * *v_x;
        *v_yy = *v_y * *v_y;
        *v_xy = *v_x * *v_y;

    }
    image g = make_gaussian_filter(sigma);
    image weight = convolve_image(im, g, 0);

    for (int i = 0;i < S.h;i++) {
        for (int j = 0;j < S.w;j++) {
            set_pixel(S, j, i, 0, get_pixel(weight, j, i, 0) * get_pixel(IxIx, j, i, 0));
            set_pixel(S, j, i, 1, get_pixel(weight, j, i, 0) * get_pixel(IyIy, j, i, 0));
            set_pixel(S, j, i, 2, get_pixel(weight, j, i, 0) * get_pixel(IxIy, j, i, 0));
        }
    }
    return S;
}
// Estimate the cornerness of each pixel given a structure matrix S.
// image S: structure matrix for an image.
// returns: a response map of cornerness calculations.
float det(matrix A) {
    float determinant = A.data[0][0] * A.data[1][1] - A.data[1][0] * A.data[0][1];
    return determinant;
}

float trace(matrix A) {
    float trace = A.data[0][0] + A.data[1][1];
    return trace;
}

image cornerness_response(image S)
{
    printf("cornerness_response");
    image R = make_image(S.w, S.h, 1);
    // TODO: fill in R, "cornerness" for each pixel using the structure matrix.
    // We'll use formulation det(S) - alpha * trace(S)^2, alpha = .06.
    float alpha = 0.06;
    float value;
    matrix s = make_matrix(2, 2);
    for (int i = 0;i < S.h;i++) {
        for (int j = 0;j < S.w;j++) {
            s.data[0][0] = get_pixel(S, j, i, 0);
            s.data[0][1] = get_pixel(S, j, i, 2);
            s.data[1][0] = get_pixel(S, j, i, 2);
            s.data[1][1] = get_pixel(S, j, i, 1);
            value = det(s) - alpha * trace(s) * trace(s);
            set_pixel(R, j, i, 0, value);
        }
    }
    return R;
}

// Perform non-max supression on an image of feature responses.
// image im: 1-channel image of feature responses.
// int w: distance to look for larger responses.
// returns: image with only local-maxima responses within w pixels.
image nms_image(image im, int w)
{
    printf("nms_image ongoing");
    image r = copy_image(im);
    float max;
    for (int a = 0;a < r.h;a++) {
        for (int b = 0;b < r.w;b++) {
            for (int i = 0;i < 2 * w + 1;i++) {
                for (int j = 0;j < 2 * w + 1;j++) {
                    max = (get_pixel(r, b - j - w, a - i - w, 0) <= get_pixel(r, b, a, 0)) ? get_pixel(r, b, a, 0) : -9999;
                    set_pixel(r, j, i, 0, max);
                }
            }

        }
    }
    // for every pixel in the image:
    //     for neighbors within w:
    //         if neighbor response greater than pixel response:
    //             set response to be very low (I use -999999 [why not 0??])
    return r;
}

// Perform harris corner detection and extract features from the corners.
// image im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
// int *n: pointer to number of corners detected, should fill in.
// returns: array of descriptors of the corners in the image.
descriptor* harris_corner_detector(image im, float sigma, float thresh, int nms, int* n)
{
    // Calculate structure matrix
    printf("inside corner detector");

    image S = structure_matrix(im, sigma);

    // Estimate cornerness
    image R = cornerness_response(S);

    // Run NMS on the responses
    image Rnms = nms_image(R, nms);


    //TODO: count number of responses over threshold
    int count = 0;
    for (int i = 0;i < Rnms.h * Rnms.w * Rnms.c;i++) {
        if (Rnms.data[i] >= thresh) {
            count += 1;
        }
    }
    // change this


    *n = count; // <- set *n equal to number of corners in image.
    descriptor* d = calloc(count, sizeof(descriptor));

    int a = 0;
    //TODO: fill in array *d with descriptors of corners, use describe_index.

    while (a >= count) {
        for (int i = 0;i < Rnms.h * Rnms.w * Rnms.c;i++) {
            if (Rnms.data[i] > thresh) {
                d[a] = describe_index(im, i);
                a += 1;
            }
        }
    }
    free_image(S);
    free_image(R);
    free_image(Rnms);
    return d;
}

// Find and draw corners on an image.
// image im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
void detect_and_draw_corners(image im, float sigma, float thresh, int nms)
{
    printf("initializing");
    int n = 0;
    descriptor* d = harris_corner_detector(im, sigma, thresh, nms, &n);
    mark_corners(im, d, n);
}
