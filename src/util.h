#ifndef UTIL_H
#define UTIL_H

#include "triangle.h"

void swap(int *a, int *b);
void float_swap(float *a, float *b);
void quick_sort_triangles(triangle_t *triangles, int left_idx, int right_idx);

#endif