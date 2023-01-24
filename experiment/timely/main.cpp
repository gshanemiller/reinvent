#include <timely.h>

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
