#ifndef __LIB_LIBPERF_H
#define __LIB_LIBPERF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>                                                                                                  

struct perf {
  int fd1;
  int fd2;
  int fd3;
  int fd4;
  int fd5;
  uint64_t id1;
  uint64_t id2;
  uint64_t id3;
  uint64_t id4;
  uint64_t id5;
};

struct read_format {
  uint64_t nr;
  struct {
    uint64_t value;
    uint64_t id;
  } values[];
};

int perf_initialize(int cpu);
int perf_on();
int perf_off();
int perf_read();
int perf_close();

#ifdef __cplusplus
}
#endif

#endif /* __LIBPERF_H */
