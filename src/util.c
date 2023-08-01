#include "util.h"
#include "triangle.h"

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
};

void swap_triangle(triangle_t *a, triangle_t *b)
{
    triangle_t temp = *a;
    *a = *b;
    *b = temp;
};

int partition(triangle_t *triangles, int left_idx, int right_idx)
{
    // Choosing the pivot
    float pivot = triangles[right_idx].avg_depth;

    // Index of smaller element and indicates
    // the right position of pivot found so far
    int i = (left_idx - 1);

    for (int j = left_idx; j <= right_idx - 1; j++)
    {

        // If current element is greater than the pivot
        if (triangles[j].avg_depth > pivot)
        {

            // Increment index of greater element
            i++;
            swap_triangle(&triangles[i], &triangles[j]);
        }
    }
    swap_triangle(&triangles[i + 1], &triangles[right_idx]);
    return (i + 1);
}

void quick_sort_triangles(triangle_t *triangles, int left_idx, int right_idx)
{
    if (left_idx < right_idx)
    {

        // pi is partitioning index, triangles[p]
        // is now at right place
        int pi = partition(triangles, left_idx, right_idx);

        // Separately sort elements before
        // partition and after partition
        quick_sort_triangles(triangles, left_idx, pi - 1);
        quick_sort_triangles(triangles, pi + 1, right_idx);
    }
};
