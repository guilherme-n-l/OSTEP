#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AVG(A, B) ((A) / (double)(B))
#define MED(A, B)                                                              \
  ((B) & 1 ? (A)[(size_t)((B) / 2)]                                            \
           : ((A)[(size_t)((B) / 2)] + (A)[(size_t)((B) / 2 - 1)]) / 2.)
#define N_MIDDLE_TITLE "Middle %zu values"

typedef struct timespec timespec;

int qsort_asc(const void *a_ptr, const void *b_ptr) {
  time_t a = *(const time_t *)a_ptr;
  time_t b = *(const time_t *)b_ptr;
  return (a > b) - (a < b);
}

void show_n_middle(const time_t *arr, size_t arr_length, size_t n,
                   char *description) {
  if (!arr || !arr_length || n == 0) {
    printf("Nothing to show.\n");
    return;
  }

  if (description)
    printf(N_MIDDLE_TITLE " (%s):\n", arr_length < n ? arr_length : n,
           description);
  else
    printf(N_MIDDLE_TITLE ":\n", arr_length < n ? arr_length : n);

  if (arr_length < n)
    for (size_t i = 0; i < arr_length; i++)
      printf("\t%.2lf\n", (double)arr[i]);
  else {
    size_t start = (n / 2 > arr_length / 2) ? 0 : (arr_length / 2 - n / 2),
           end = start + n;

    for (size_t i = start; i < end; i++)
      printf("[exp: %zu] %.2lf\n", i, (double)arr[i]);
  }
}

time_t timeit(timespec *start_ptr, timespec *end_ptr) {
  if (!end_ptr || !start_ptr) {
    perror("Trying to deference NULL pointer, returning 0");
    exit(1);
  }

  timespec start = *start_ptr, end = *end_ptr;
  return (end.tv_sec - start.tv_sec) * (time_t)1e9 +
         (end.tv_nsec - start.tv_nsec);
}
