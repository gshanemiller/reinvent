#include <common.h>

uint64_t timeDifference(timespec start, timespec end) {                                                                 
  uint64_t diff = static_cast<uint64_t>(end.tv_sec)*static_cast<uint64_t>(1000000000)+static_cast<uint64_t>(end.tv_nsec);
  diff -= (static_cast<uint64_t>(start.tv_sec)*static_cast<uint64_t>(1000000000)+static_cast<uint64_t>(start.tv_nsec)); 
  return diff;                                                                                                          
}
