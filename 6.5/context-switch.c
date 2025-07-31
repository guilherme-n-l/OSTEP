#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define if_in_fork(A) if (!(A))
#define CH_READ 0
#define CH_WRITE 1
#define N_EXPERIMENTS (size_t)1e6

void ensure_fork(pid_t *pid_ptr) {
  if (!pid_ptr || *pid_ptr < 0) {
    perror("Could not fork parent process in this experiment");
    exit(1);
  }
}

void ensure_pipe(int (*pipe_ptr)[2]) {
  if (!pipe_ptr || pipe(*pipe_ptr)) {
    perror("Could not create pipe");
    exit(1);
  }
}

int main() {
  printf("Measuring context switch duration for %zu experiments\n\n",
         N_EXPERIMENTS);
  int status;

  int p2c[2], c2p[2];

  ensure_pipe(&p2c), ensure_pipe(&c2p);

  pid_t ppid = fork();
  ensure_fork(&ppid);

  if_in_fork(ppid) {
    close(p2c[CH_WRITE]), close(c2p[CH_READ]);

    char buf = 'c';
    for (size_t i = 0; i < N_EXPERIMENTS; i++) {
      read(p2c[CH_READ], &buf, sizeof(char));
      write(c2p[CH_WRITE], &buf, sizeof(char));
    }

    close(p2c[CH_READ]), close(c2p[CH_WRITE]);
  }
  else {
    close(p2c[CH_READ]), close(c2p[CH_WRITE]);

    struct timespec start, end;
    long long total_time = 0;
    time_t experiments[N_EXPERIMENTS];

    char buf = 'p';
    for (size_t i = 0; i < N_EXPERIMENTS; i++) {
      clock_gettime(CLOCK_MONOTONIC_RAW, &start);

      write(p2c[CH_WRITE], &buf, sizeof(char));
      read(c2p[CH_READ], &buf, sizeof(char));

      clock_gettime(CLOCK_MONOTONIC_RAW, &end);

      experiments[i] = (end.tv_sec - start.tv_sec) * (time_t)1e9 +
                       (end.tv_nsec - start.tv_nsec);
      total_time += experiments[i];
    }

    close(p2c[CH_WRITE]), close(c2p[CH_READ]);
    wait(&status);

    qsort(experiments, N_EXPERIMENTS, sizeof(time_t), qsort_asc);

    printf("AVG: %.2lf ns\n", (double)total_time / N_EXPERIMENTS);
    printf("MED: %.2lf ns\n",
           N_EXPERIMENTS & 1 ? experiments[(size_t)(N_EXPERIMENTS / 2)]
                             : (experiments[(size_t)(N_EXPERIMENTS / 2)] +
                                experiments[(size_t)(N_EXPERIMENTS / 2 - 1)]) /
                                   2.);

    printf("Estimated single context switch time: %.2lf ns (AVG)\n",
           (double)total_time / N_EXPERIMENTS / 2);
    printf("Estimated single context switch time: %.2lf ns (MED)\n",
           N_EXPERIMENTS & 1 ? experiments[(size_t)(N_EXPERIMENTS / 2)] / 2.
                             : (experiments[(size_t)(N_EXPERIMENTS / 2)] +
                                experiments[(size_t)(N_EXPERIMENTS / 2 - 1)]) /
                                   2. / 2.);

    size_t median_index = N_EXPERIMENTS / 2;

    if (N_EXPERIMENTS > 5) {
      printf("\nMiddle 5 values:\n");

      size_t start_index = (median_index >= 2) ? (median_index - 2) : 0;
      size_t end_index = (median_index + 2 < N_EXPERIMENTS)
                             ? (median_index + 2)
                             : (N_EXPERIMENTS - 1);

      for (size_t i = start_index; i <= end_index; i++)
        printf("\texperiment %zu: %.2lf ns\n", i + 1, (double)experiments[i]);
    } else {
      printf("\nExperiments:\n");

      for (size_t i = 0; i < N_EXPERIMENTS; i++)
        printf("\texperiment %zu: %.2lf ns\n", i + 1, (double)experiments[i]);
    }
  }

  return 0;
}
