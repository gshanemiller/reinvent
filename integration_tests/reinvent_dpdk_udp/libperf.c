#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <linux/perf_event.h>
#include <sys/ioctl.h>

#include "libperf.h"

static struct perf libperf;

int perf_initialize(int cpu) {
  struct perf_event_attr pea;

  memset(&pea, 0, sizeof(struct perf_event_attr));
  pea.type = PERF_TYPE_HARDWARE;
  pea.size = sizeof(struct perf_event_attr);
  pea.config = PERF_COUNT_HW_CPU_CYCLES;
  pea.disabled = 1;
  pea.exclude_kernel = 1;
  pea.exclude_hv = 1;
  pea.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;
  libperf.fd1 = syscall(__NR_perf_event_open, &pea, 0, cpu, -1, 0);
  ioctl(libperf.fd1, PERF_EVENT_IOC_ID, &libperf.id1);
  ioctl(libperf.fd1, PERF_EVENT_IOC_ID, &libperf.id1);

  memset(&pea, 0, sizeof(struct perf_event_attr));
  pea.type = PERF_TYPE_HARDWARE;
  pea.size = sizeof(struct perf_event_attr);
  pea.config = PERF_COUNT_HW_CACHE_MISSES;
  pea.disabled = 1;
  pea.exclude_kernel = 1;
  pea.exclude_hv = 1;
  pea.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;
  libperf.fd2 = syscall(__NR_perf_event_open, &pea, 0, cpu, libperf.fd1, 0);
  ioctl(libperf.fd2, PERF_EVENT_IOC_ID, &libperf.id2);

  memset(&pea, 0, sizeof(struct perf_event_attr));
  pea.type = PERF_TYPE_HARDWARE;
  pea.size = sizeof(struct perf_event_attr);
  pea.config = PERF_COUNT_HW_CACHE_REFERENCES;
  pea.disabled = 1;
  pea.exclude_kernel = 1;
  pea.exclude_hv = 1;
  pea.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;
  libperf.fd3 = syscall(__NR_perf_event_open, &pea, 0, cpu, libperf.fd1, 0);
  ioctl(libperf.fd3, PERF_EVENT_IOC_ID, &libperf.id3);

  memset(&pea, 0, sizeof(struct perf_event_attr));
  pea.type = PERF_TYPE_HARDWARE;
  pea.size = sizeof(struct perf_event_attr);
  pea.config = PERF_COUNT_HW_STALLED_CYCLES_FRONTEND;
  pea.disabled = 1;
  pea.exclude_kernel = 1;
  pea.exclude_hv = 1;
  pea.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;
  libperf.fd4 = syscall(__NR_perf_event_open, &pea, 0, cpu, libperf.fd1, 0);
  ioctl(libperf.fd4, PERF_EVENT_IOC_ID, &libperf.id4);

  memset(&pea, 0, sizeof(struct perf_event_attr));
  pea.type = PERF_TYPE_HARDWARE;
  pea.size = sizeof(struct perf_event_attr);
  pea.config = PERF_COUNT_HW_STALLED_CYCLES_BACKEND;
  pea.disabled = 1;
  pea.exclude_kernel = 1;
  pea.exclude_hv = 1;
  pea.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;
  libperf.fd5 = syscall(__NR_perf_event_open, &pea, 0, cpu, libperf.fd1, 0);
  ioctl(libperf.fd5, PERF_EVENT_IOC_ID, &libperf.id5);

  ioctl(libperf.fd1, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
  return 0;
}

int perf_on() {
  ioctl(libperf.fd1, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);
  return 0;
}

int perf_off() {
  ioctl(libperf.fd1, PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP);
  return 0;
}

int perf_read() {
  char buf[16384];
  struct read_format* rf = (struct read_format*)buf;

  unsigned long sz = read(libperf.fd1, buf, sizeof(buf));
  printf("%lu results, read %lu bytes\n", rf->nr, sz);
  for (int i = 0; i < rf->nr; i++) {
    if (rf->values[i].id == libperf.id1) {
      printf("PERF_COUNT_HW_CPU_CYCLES: %lu\n", rf->values[i].value);
    } else if (rf->values[i].id == libperf.id2) {
      printf("PERF_COUNT_HW_CACHE_MISSES: %lu\n", rf->values[i].value);
    } else if (rf->values[i].id == libperf.id3) {
      printf("PERF_COUNT_HW_CACHE_REFERENCES: %lu\n", rf->values[i].value);
    } else if (rf->values[i].id == libperf.id4) {
      printf("PERF_COUNT_HW_STALLED_CYCLES_FRONTEND: %lu\n", rf->values[i].value);
    } else if (rf->values[i].id == libperf.id5) {
      printf("PERF_COUNT_HW_STALLED_CYCLES_BACKEND: %lu\n", rf->values[i].value);
    }
  }
}

int perf_close() {
  close(libperf.fd1);
  close(libperf.fd2);
  close(libperf.fd3);
  close(libperf.fd4);
  close(libperf.fd5);
  return 0;
}
