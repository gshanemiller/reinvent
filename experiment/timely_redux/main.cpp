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
  pinToHWCore(sched_getcpu());

  const double ghz = Reinvent::measure_rdtsc_freq();
  const double bps = 1250000000.0; // NIC line rate 10Gbps as bps

  printf("estimated CPU frequency: %lf\n", ghz);
  printf("maximum linkRate: %lf bps\n", bps);

  double rttUs = 10.0;

  // The Timely TX rate estimator
  Reinvent::Timely timely(ghz, bps, 0);

  unsigned iters = 50;
  while(--iters) {
      timely.newRate(rttUs);
      printf("rttUs: %lf us, new rate (Gbps): %lf\n", rttUs, timely.lastRateAsGbps());
      rttUs += 25.0;
  }

  iters = 50;
  while(--iters) {
      rttUs -= 25.0;
      timely.newRate(rttUs);
      printf("rttUs: %lf us, new rate (Gbps): %lf\n", rttUs, timely.lastRateAsGbps());
  }

  std::cout << timely;

  return 0;
}
