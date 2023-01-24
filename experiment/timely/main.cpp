#include <timely.h>
#include <sched.h>

// rdtsc pointless unless pinned
int pinToHWCore(int coreId) {
  assert(coreId>=0);

  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(coreId, &mask);

  if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1) {
      return errno;
  }

  return 0;

int main(int argc, char **argv) {
  unsigned iters = 10;
  const double ghz = erpc::measure_rdtsc_freq();
  const double bps = 1342177280; // 10Gbps as bps

  printf("sizeof(size_t)=%lu\n", sizeof(size_t));
  printf("estimated CPU frequency: %lf\n", ghz);
  printf("initial linkRate: %lf bps, %lf Gbps\n", bps, erpc::Timely::rate_to_gbps(bps));

  size_t sleep = 1000;
  erpc::Timely timely(ghz, bps);

  auto lastTs = erpc::rdtsc();

  while(--iters) {
      erpc::nano_sleep(sleep, ghz);
      auto nowTs = erpc::rdtsc();
      timely.update_rate(nowTs, nowTs-lastTs);
      printf("delayRdtsc: %lu new rate (gbps): %lf\n", nowTs-lastTs, timely.get_rate_gbps());
      sleep <<= 1;
      lastTs = nowTs;
  }

  iters = 1000;
  while(--iters) {
      erpc::nano_sleep(sleep, ghz);
      auto nowTs = erpc::rdtsc();
      timely.update_rate(nowTs, nowTs-lastTs);
      printf("delayRdtsc: %lu new rate (gbps): %lf\n", nowTs-lastTs, timely.get_rate_gbps());
      sleep >>= 1;
      lastTs = nowTs;
  }

  return 0;
}
