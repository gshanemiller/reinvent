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
  const double bps = 10000000000.0; // NIC line rate 10Gbps (giga bytes/sec) as bps

  printf("estimated CPU frequency: %lf\n", ghz);
  printf("maximum linkRate: %lf bps\n", bps);

  // The Timely TX rate estimator
  Reinvent::Timely timely(ghz, bps, 0);

  double rttUs = 10.0;
  unsigned iters = 50;
  u_int64_t now = Reinvent::rdtsc();

  while(--iters) {
      now += static_cast<u_int64_t>(rttUs*1000.0*ghz);
      timely.update(now, rttUs);
      printf("rttUs: %lf us, new rate (Gbps): %lf\n", rttUs, timely.rateAsGbps());
      rttUs += 25.0;
  }

  iters = 50;
  while(--iters) {
      rttUs -= 25.0;
      now += static_cast<u_int64_t>(rttUs*1000.0*ghz);
      timely.update(now, rttUs);
      printf("rttUs: %lf us, new rate (Gbps): %lf\n", rttUs, timely.rateAsGbps());
  }

  std::cout << timely;

  return 0;
}
