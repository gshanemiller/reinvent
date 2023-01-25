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


  // rttUs under Timele::d_minRttUs
  {
    // The Timely TX rate estimator
    Reinvent::Timely timely(ghz, bps, 0);
    const rttUs = 10;
    const auto newRate = timely.newRate(rttUs);
    assert(
  }

  return 0;
}
