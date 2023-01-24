#include <timely.h>
#include <sched.h>

int pinToHWCore(int coreId) {
  assert(coreId>=0);

  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(coreId, &mask);

  if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1) {
      return errno;
  }

  return 0;
}

int main(int argc, char **argv) {
  // rdtsc pointless unless pinned
  pinToHWCore(sched_getcpu());

  unsigned iters = 10;
  const double ghz = Reinvent::measure_rdtsc_freq();
  const double bps = 1342177280; // 10Gbps as bps

  printf("sizeof(size_t)=%lu\n", sizeof(size_t));
  printf("estimated CPU frequency: %lf\n", ghz);
  printf("initial linkRate: %lf bps\n", bps);

  size_t sleep = 1000 * ghz;
  Reinvent::Timely timely(ghz, bps, 0);

  auto lastTs = Reinvent::rdtsc();

  while(--iters) {
      Reinvent::nano_sleep(sleep);
      auto nowTs = Reinvent::rdtsc();
      const double newRate = timely.newRate(nowTs-lastTs);
      printf("delayRdtsc: %lu cycles (%lu ns) new rate (bps): %lf\n", nowTs-lastTs, sleep, newRate);
      sleep <<= 1;
      lastTs = nowTs;
  }

  iters = 11;
  while(--iters) {
      Reinvent::nano_sleep(sleep);
      auto nowTs = Reinvent::rdtsc();
      const double newRate = timely.newRate(nowTs-lastTs);
      printf("delayRdtsc: %lu cycles (%lu ns) new rate (bps): %lf\n", nowTs-lastTs, sleep, newRate);
      sleep >>= 1;
      lastTs = nowTs;
  }

  return 0;
}
