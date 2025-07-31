/*
 * 0_byte_read.c
 *
 * Description:
 *   This program measures the duration of a no-op system call (zero-byte read)
 *   on macOS (Darwin) running on AArch64 (e.g., Apple Silicon).
 *   It uses inline assembly to make a direct system call and times its duration
 *   with `clock_gettime`. The results are used to compute average and median
 *   syscall duration.
 *
 * Author: Guilherme Lima
 * Date: Aug 1. 2025
 *
 * Platform: macOS (AArch64 / Apple Silicon)
 * Build with: clang -O0 -Wall -Werror
 *
 * Notes:
 *   - Darwin does not use the same syscall numbers or calling conventions as
 * Linux.
 *   - Inline assembly must follow the macOS ABI and Clang’s constraints.
 *   - Ensure the target is Apple Silicon (ARM64) and not Intel x86_64.
 */
#include "utils.h"
#include <stdio.h>
#include <sys/syscall.h>

#define N_EXPERIMENTS (size_t)1e6
// Performs a zero-byte read syscall on stdin (fd=0) via inline asm
void read_0_bytes(const char *buffer) {
  __asm__ volatile("mov     x0,   #0\n" // stdin fd
                   "mov     x1,   %0\n" // buffer
                   "mov     x2,   #0\n" // 0 byte read
                   "mov     x16,  %1\n" // read syscall
                   "svc     #0\n"       // make syscall
                   :
                   : "r"(buffer), "i"(SYS_read) // pass buffer to %0
                   : "x0", "x1", "x2", "x16");  // clobbering registers
}

int main() {
  printf("Measuring syscall duration for %zu experiments\n\n", N_EXPERIMENTS);
  char buffer[1] = {0};

  long long total_time = 0;
  timespec start, end;
  time_t experiments[N_EXPERIMENTS];

  for (size_t i = 0; i < N_EXPERIMENTS; i++) {
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    read_0_bytes((char *)buffer);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    experiments[i] = timeit(&start, &end);
    total_time += experiments[i];
  }

  printf("AVG: %.2lf ns\n", AVG(total_time, N_EXPERIMENTS));

  qsort(experiments, N_EXPERIMENTS, sizeof(time_t), qsort_asc);
  printf("MED: %.2lf ns\n\n", MED(experiments, N_EXPERIMENTS));

  show_n_middle(experiments, N_EXPERIMENTS, 5, "in ns");

  return 0;
}
