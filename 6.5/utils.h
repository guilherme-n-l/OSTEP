#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include <stddef.h>

// Average macro: returns the average of A divided by B as a double
#define AVG(A, B) ((A) / (double)(B))

// Median macro: returns the median value from a sorted array A of length B
#define MED(A, B)                                                              \
  ((B) & 1 ? (A)[(size_t)((B) / 2)]                                            \
           : ((A)[(size_t)((B) / 2)] + (A)[(size_t)((B) / 2 - 1)]) / 2.)

typedef struct timespec timespec;

// Comparator for qsort: sorts time_t values ascending
int qsort_asc(const void *a_ptr, const void *b_ptr);

// Prints the middle n values of a sorted time_t array. Sets `description` along the title
void show_n_middle(const time_t *arr, size_t arr_length, size_t n, char *description);

// Returns elapsed time in nanoseconds between two timespec timestamps
time_t timeit(timespec *start_ptr, timespec *end_ptr);

#endif // UTILS_H

