#include "util.h"
#include "triangle.h"

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
};

void float_swap(float *a, float *b)
{
    float temp = *a;
    *a = *b;
    *b = temp;
};

void triangle_swap(triangle_t *a, triangle_t *b)
{
    triangle_t temp = *a;
    *a = *b;
    *b = temp;
};
