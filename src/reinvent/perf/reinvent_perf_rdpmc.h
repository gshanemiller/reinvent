#pragma once

// Purpose: Read and store Intel's Architectural Performance Events from its PMU (Performance Monitor Unit)
//
// Classes:
//   Perf::Rdpmc: Programs PMU counter 0=UNHALTED_CORE_CYCLES, 1=INSTRUCTIONS_RETIRED, 2=LLC_REFERENCES, 3=LLC_MISSES
//                optionally pinning caller's thread the caller's current HW core. It provides an accessor to read the
//                current value of any counter. Typically used as a base class.
//
//   Perf::RdpmcSumDelta: Building on 'Perf::Rdpmc' totals the differences in counters calculated as the delta in the
//                the counter values as 'stop()-start()'. The detlas are added to a running total per counter.
// 

#include <stdio.h>
#include <assert.h>
#include <x86intrin.h>

namespace Reinvent {
namespace Perf {

                    // ========================================
                    // Class: Rdpmc 
                    // ========================================

class Rdpmc {
public:
  // ENUM
  enum Event {
    // See https://github.com/rodgarrison/rdpmc for details on
    // where these magic constants come from
    UNHALTED_CORE_CYCLES      = 0x003c,     // counter 0 event
    INSTRUCTIONS_RETIRED      = 0x00c0,     // counter 1 event
    LLC_REFERENCES            = 0x4f2e,     // counter 2 event
    LLC_MISSES                = 0x412e,     // counter 3 event
  };
 
  // CREATORS
  Rdpmc(bool pin);
    // Create Rdpmc object to track PMU events in four counters 0=UNHALTED_CORE_CYCLES, 1=INSTRUCTIONS_RETIRED,
    // 2=LLC_REFERENCES, 3=LLC_MISSES. If 'pin' true, the caller's thread is pinned to the HW core it's running on.
    // Set 'pin=false' if caller's thread is already pinned. The caller's thread must be pinned to a HW core since
    // PMU counters are per core. Counters are initialized to zero and, upon return, the counters are running.
 
  Rdpmc(const Rdpmc &other) = delete;
    // Copy constructor not provided.

  ~Rdpmc() = default;
    // Destroy this object
 
  // ACCESSORS
  int cpu() const;
    // Return the zero-based HW core number the caller is running on

  unsigned long readCounter(int cntr);
    // Read and return the current value of the specified '0<=cntr<=3' counter. Callers typically run this before the
    // benchmark code then after. The difference in the counter values represents the number of events which occurred
    // as the benchmark code ran.

private:
  // PRIVATE MANIPULATORS
  int rdmsr(unsigned int reg, int cpu, unsigned long *value);
    // Return zero if read from specified Intel PMU register 'reg' on specified 'cpu' current value writing it into
    // 'value' and non-zero otherwise;
  
  int wrmsr(unsigned int reg, int cpu, unsigned long data);
    // Return zero if wrote to specified Intel PMU register 'reg' on specified 'cpu' the specified value 'data' and
    // non-zero otherwise

  int pinCpu();
    // Return 0 if the caller's thread was pinned to the CPU returned by 'cpu()' and non-zero otherwise.

public:
  // PUBLIC MANIPULATORS
  const Rdpmc& operator=(const Rdpmc& rhs) = delete;
    // Assignment operator not provided.
};

// ACCESSORS
inline
__attribute__((optimize("O0")))
unsigned long Rdpmc::readCounter(int cntr) {
  assert(cntr>=0&&cntr<=3);
  __asm __volatile("lfence");
  return __rdpmc(cntr);
}

                    // ========================================
                    // Class: RdpmcSumDelta
                    // ========================================

class RdpmcSumDelta: public Rdpmc {
private:
  unsigned long d_lastCounter0Value;    // value of counter 0 last time read
  unsigned long d_lastCounter1Value;    // value of counter 1 last time read
  unsigned long d_lastCounter2Value;    // value of counter 2 last time read
  unsigned long d_lastCounter3Value;    // value of counter 3 last time read
  unsigned long d_sumDelteCounter0;     // running sum of deltas between 'stop()-start()' for counter 0
  unsigned long d_sumDelteCounter1;     // running sum of deltas between 'stop()-start()' for counter 1
  unsigned long d_sumDelteCounter2;     // running sum of deltas between 'stop()-start()' for counter 2
  unsigned long d_sumDelteCounter3;     // running sum of deltas between 'stop()-start()' for counter 3

public:
  // CREATORS
  RdpmcSumDelta(bool pin);
    // Create RdpmcSumDelta object to track PMU events as described in the base class default constructor.
 
  RdpmcSumDelta(const Rdpmc &other) = delete;
    // Copy constructor not provided.

  ~RdpmcSumDelta() = default;
    // Destroy this object
 
  // ACCESSORS
  unsigned long sumDeltaCounter0() const;
    // Return the current sum of all the deltas taken between 'stop()-start()' for counter 0. See base class for
    // what counter 0 represents.

  unsigned long sumDeltaCounter1() const;
    // Return the current sum of all the deltas taken between 'stop()-start()' for counter 1. See base class for
    // what counter 1 represents.

  unsigned long sumDeltaCounter2() const;
    // Return the current sum of all the deltas taken between 'stop()-start()' for counter 2. See base class for
    // what counter 2 represents.
  
  unsigned long sumDeltaCounter3() const;
    // Return the current sum of all the deltas taken between 'stop()-start()' for counter 3. See base class for
    // what counter 3 represents.

public:
  // PUBLIC MANIPULATORS
  void start();
    // Read and cache the current values of all four counters. Use with 'stop()'.

  void stop();
    // Read the current values of all four counters then add to the running sum by counter the difference of the
    // current value minus the value cached when 'start()' was last called.

  const RdpmcSumDelta& operator=(const RdpmcSumDelta& rhs) = delete;
    // Assignment operator not provided.
};

// CREATORS
inline
RdpmcSumDelta::RdpmcSumDelta(bool pin)
: Rdpmc(pin)
, d_lastCounter0Value(0)
, d_lastCounter1Value(0)
, d_lastCounter2Value(0)
, d_lastCounter3Value(0)
, d_sumDelteCounter0(0)
, d_sumDelteCounter1(0)
, d_sumDelteCounter2(0)
, d_sumDelteCounter3(0)
{
}

// ACCESSORS
inline
unsigned long RdpmcSumDelta::sumDeltaCounter0() const {
  return d_sumDelteCounter0;
}

inline
unsigned long RdpmcSumDelta::sumDeltaCounter1() const {
  return d_sumDelteCounter1;
}

inline
unsigned long RdpmcSumDelta::sumDeltaCounter2() const {
  return d_sumDelteCounter2;
}
  
inline
unsigned long RdpmcSumDelta::sumDeltaCounter3() const {
  return d_sumDelteCounter3;
}

inline
__attribute__((optimize("O0")))
void RdpmcSumDelta::start() {
  __asm __volatile("lfence");
  d_lastCounter0Value = __rdpmc(0);
  d_lastCounter1Value = __rdpmc(1);
  d_lastCounter2Value = __rdpmc(2);
  d_lastCounter3Value = __rdpmc(3);
}

inline
__attribute__((optimize("O0")))
void RdpmcSumDelta::stop() {
  __asm __volatile("lfence");
  d_sumDelteCounter0 += (__rdpmc(0)-d_lastCounter0Value);
  d_sumDelteCounter1 += (__rdpmc(1)-d_lastCounter1Value);
  d_sumDelteCounter2 += (__rdpmc(2)-d_lastCounter2Value);
  d_sumDelteCounter3 += (__rdpmc(3)-d_lastCounter3Value);
}

} // namespace Perf
} // namespace Reinvent 
