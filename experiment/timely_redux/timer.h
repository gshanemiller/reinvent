#pragma once

#include <sys/types.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

namespace Reinvent {

// Return the TSC. Caller must be HW core pinned
static inline uint64_t rdtsc() {
  uint64_t rax;
  uint64_t rdx;
  asm volatile("rdtsc" : "=a"(rax), "=d"(rdx));
  return static_cast<size_t>((rdx << 32) | rax);
}

// Nano sleep using using rdtsc() to count elapsed time
// Caller HW core must be pinned. The parameter 'sleepCycles'
// is calculated as desired sleep time in ns multiplied by
// CPU's frequency. Function returns last value of rdtsc.
static uint64_t nano_sleep(const uint64_t sleepCycles) {
  uint64_t start = rdtsc();
  uint64_t end = start;
  while (end - start < sleepCycles) end = rdtsc();
  return end;
}

// Estimate CPU frequency. Caller's thread must be pinned to a core
static double measure_rdtsc_freq() {
  struct timespec startTs, endTs;

  // Get start wall time and current cycle count
  clock_gettime(CLOCK_MONOTONIC, &startTs);
  const uint64_t rdtsc_start = rdtsc();

  // Do not change this loop! The hardcoded value below depends on this loop
  // and prevents it from being optimized out.
  uint64_t sum = 5;
  for (uint64_t i = 0; i < 1000000; i++) {
    sum += i + (sum + i) * (i % sum);
  }
  assert(sum==13580802877818827968ull);

  // Get end wall time and current cycle count difference
  clock_gettime(CLOCK_MONOTONIC, &endTs);
  const uint64_t rdtsc_cycles = rdtsc() - rdtsc_start;

  // CPU frequency estimated to be elapsed cycles divided by elapsed time in ns.
  // Note 1 sec = 1'000'000'000 ns and GigaHertz = 1'000'000'000 cycles/second
  const u_int64_t secToNs = 1000000000UL;
  const uint64_t elapsedNs = (endTs.tv_sec*secToNs+endTs.tv_nsec) - (startTs.tv_sec*secToNs+startTs.tv_nsec);
  const double freq_ghz = (rdtsc_cycles*1.0) / (elapsedNs*1.0);
  assert(freq_ghz >= 0.5 && freq_ghz <= 5.0);

  return freq_ghz;
}

}  // namespace Reinvent
