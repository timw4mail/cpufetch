#ifdef __linux__

#define _GNU_SOURCE

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <asm/unistd.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>

#include "global.h"
#include "cpu.h"

static long
perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                int cpu, int group_fd, unsigned long flags) {
    int ret;
    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                    group_fd, flags);
    return ret;
}

#define INSERT_ASM_ONCE __asm volatile("nop");
#define INSERT_ASM_10_TIMES \
    INSERT_ASM_ONCE         \
    INSERT_ASM_ONCE         \
    INSERT_ASM_ONCE         \
    INSERT_ASM_ONCE         \
    INSERT_ASM_ONCE         \
    INSERT_ASM_ONCE         \
    INSERT_ASM_ONCE         \
    INSERT_ASM_ONCE         \
    INSERT_ASM_ONCE         \
    INSERT_ASM_ONCE         \

#define INSERT_ASM_100_TIMES \
    INSERT_ASM_10_TIMES      \
    INSERT_ASM_10_TIMES      \
    INSERT_ASM_10_TIMES      \
    INSERT_ASM_10_TIMES      \
    INSERT_ASM_10_TIMES      \
    INSERT_ASM_10_TIMES      \
    INSERT_ASM_10_TIMES      \
    INSERT_ASM_10_TIMES      \
    INSERT_ASM_10_TIMES      \
    INSERT_ASM_10_TIMES

#define INSERT_ASM_1000_TIMES \
    INSERT_ASM_100_TIMES     \
    INSERT_ASM_100_TIMES     \
    INSERT_ASM_100_TIMES     \
    INSERT_ASM_100_TIMES     \
    INSERT_ASM_100_TIMES     \
    INSERT_ASM_100_TIMES     \
    INSERT_ASM_100_TIMES     \
    INSERT_ASM_100_TIMES     \
    INSERT_ASM_100_TIMES     \
    INSERT_ASM_100_TIMES     \

void nop_function(uint64_t iters) {
  for (uint64_t i = 0; i < iters; i++) {
    INSERT_ASM_1000_TIMES
    INSERT_ASM_1000_TIMES
    INSERT_ASM_1000_TIMES
    INSERT_ASM_1000_TIMES
  }
}

// Run the nop_function with the number of iterations specified and
// measure both the time and number of cycles
int measure_freq_iters(uint64_t iters, uint32_t core, double* freq) {
  clockid_t clock = CLOCK_PROCESS_CPUTIME_ID;
  struct timespec start, end;
  struct perf_event_attr pe;
  uint64_t cycles;
  int fd;
  int pid = 0;
  memset(&pe, 0, sizeof(struct perf_event_attr));
  pe.type = PERF_TYPE_HARDWARE;
  pe.size = sizeof(struct perf_event_attr);
  pe.config = PERF_COUNT_HW_CPU_CYCLES;
  pe.disabled = 1;
  pe.exclude_kernel = 1;
  pe.exclude_hv = 1;

  fd = perf_event_open(&pe, pid, core, -1, 0);
  if (fd == -1) {
    perror("perf_event_open");
    if (errno == EPERM || errno == EACCES) {
      printErr("You may not have permission to collect stats.\n"\
      "Consider tweaking /proc/sys/kernel/perf_event_paranoid or running as root");
    }
    return -1;
  }

  if (clock_gettime(clock, &start) == -1) {
    perror("clock_gettime");
    return -1;
  }
  if(ioctl(fd, PERF_EVENT_IOC_RESET, 0) == -1) {
    perror("ioctl");
    return -1;
  }
  if(ioctl(fd, PERF_EVENT_IOC_ENABLE, 0) == -1) {
    perror("ioctl");
    return -1;
  }

  nop_function(iters);

  ssize_t ret = read(fd, &cycles, sizeof(uint64_t));
  if (ret == -1) {
    perror("read");
    return -1;
  }
  if (ret != sizeof(uint64_t)) {
    printErr("Read returned %d, expected %d", ret, sizeof(uint64_t));
    return -1;
  }
  if(ioctl(fd, PERF_EVENT_IOC_DISABLE, 0) == -1) {
    perror("ioctl");
    return -1;
  }
  if (clock_gettime(clock, &end) == -1) {
    perror("clock_gettime");
    return -1;
  }

  uint64_t nsecs = (end.tv_sec*1e9 + end.tv_nsec) - (start.tv_sec*1e9 + start.tv_nsec);
  uint64_t usecs = nsecs/1000;  
  *freq = cycles/((double)usecs);
  return 0;
}

// Return a good number of iterations to run the nop_function in
// order to get a precise measurement of the frequency without taking
// too much time.
uint64_t get_num_iters_from_freq(double frequency) {
  // Truncate to reduce variability
  uint64_t freq_trunc = ((uint64_t) frequency / 100) * 100;
  uint64_t osp_per_iter = 4 * 1000;

  return freq_trunc * 1e7 * 1/osp_per_iter;
}

// Differences between x86 measure_frequency and this measure_max_frequency:
// - measure_frequency employs all cores simultaneously whereas
//   measure_max_frequency only employs 1.
// - measure_frequency runs the computation and checks /proc/cpuinfo whereas
//   measure_max_frequency does not rely on /proc/cpuinfo and simply
//   counts cpu cycles to measure frequency.
// - measure_frequency uses actual computation while measuring the frequency
//   whereas measure_max_frequency uses nop instructions. This makes the former
//   x86 dependant whereas the latter is architecture independant.
int64_t measure_max_frequency(uint32_t core) {
  if (!bind_to_cpu(core)) {
    printErr("Failed binding the process to CPU %d", core);
    return UNKNOWN_DATA;
  }

  // First, get very rough estimation of clock cycle to
  // compute a reasonable value for the iterations
  double estimation_freq, frequency;
  uint64_t iters = 100000;
  if (measure_freq_iters(iters, core, &estimation_freq) == -1)
    return UNKNOWN_DATA;

  if (estimation_freq <= 0.0) {
    printErr("First frequency measurement yielded an invalid value: %f", estimation_freq);
    return UNKNOWN_DATA;
  }
  iters = get_num_iters_from_freq(estimation_freq);
  printWarn("Running frequency measurement with %ld iterations on core %d...", iters, core);

  // Now perform actual measurement
  const char* frequency_banner = "cpufetch is measuring the max frequency...";
  printf("%s", frequency_banner);
  fflush(stdout);

  if (measure_freq_iters(iters, core, &frequency) == -1)
    return UNKNOWN_DATA;
  
  // Clean screen once measurement is finished
  printf("\r%*c\r", (int) strlen(frequency_banner), ' ');
  
  // Discard last digit in the frequency, which should help providing
  // more reliable and predictable values.
  return (((int) frequency + 5)/10) * 10;
}

#endif // #ifdef __linux__
