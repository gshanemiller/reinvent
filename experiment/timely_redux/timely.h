#pragma once

// Purpose: Estimate TX rate in bytes/sec for next transmission based on last RTT using the Timely algorithm
// 
// Classes:
//   Reinvent::Timely: Implements Timely 
//
// Thread Safety: not-thread-safe.
//
// Exception Policy: No exceptions
//
// This work is based on:
// 
// * [TIMELY: RTT-based Congestion Control for the Datacenter](https://conferences.sigcomm.org/sigcomm/2015/pdf/papers/p537.pdf)
// * [TIMELY Power Point Slides](http://radhikam.web.illinois.edu/TIMELY-sigcomm-talk.pdf)
// * [TIMELY Source Code from Author](http://people.eecs.berkeley.edu/~radhika/timely-code-snippet.cc) referenced and used in eRPC
// * [ECN or Delay: Lessons Learnt from Analysis of DCQCN and TIMELY](http://yibozhu.com/doc/ecndelay-conext16.pdf). Contains Timely bug-fix 
// * [Receiver-Driven RDMA Congestion Control by Differentiating Congestion Types in Datacenter Networks](https://icnp21.cs.ucr.edu/papers/icnp21camera-paper45.pdf)
// * [eRPC Slides ](https://www.usenix.org/sites/default/files/conference/protected-files/nsdi19_slides_kalia.pdf) discuss Timely p15-16
//
// Note the "ECN or Delay" paper includes a Timely algorithm correction used here. Further note Timely documents its
// model constants in section 6 [p545]:
//
//      Unless mentioned otherwise, we use the following parameters for TIMELY: segment size of 16 KB, Tlow of 50 µs,
//      Thigh of 500 µs, additive increment of 10 Mbps, and a multiplicative decrement factor (β) of 0.8.
//
// In this code the additive increment is delta. The alpha factor is less clear. In section 6.1 [p546] Timely writes:
//
//      In [46], α is the number of packets of each flow pending in the network. We convert α to a throughput value
//      since TIMELY is rate-based. Table 1 shows our results with three different values of α. For a small α = 10
//      Mbps, FAST* can achieve low tail latency 49us albeit at a significant throughput loss – achieving only 7.5
//      Gbps out of 20 Gbps line rate. At larger values (α = 50 Mbps and 100 Mbps), TIMELY still achieves better
//      throughput and latency trade-offs.
//
// eRPC's alpha value is 0.46 whereas Timely's table 1 shows 19.4, 61, 116. On the other hand Timely's own algorithm
// has the subexpression (1.0-α) suggesting 0<=α<=1. This needs more research.
//
// New transmission rates are based on the last RTT time 'r' in us which operates in a band '[tlow, tigh]'. Section
// 4.3 pg542 gives this intuition:
//
//      0                   tlow                  thigh
//      +--------------------+----------------------+---------------->
//      | additive increase  | gradient based +/-   | multiplicative
//      | to new rate        | change to new rate   | decrease in new rate
//
// Unfortunately RTT does not seem to be as simple as `end-start` where start is some timestamp when a packet was
// sent and end is the timestamp when the receiver's ACK comes back. Section 3.1 pg540 records:
//
//      RTT = completionTime - sendTime - (packet seg. size)/(NIC line rate)
//
// The subtracted amount accounts for serialization e.g. the amount of time it takes to put the packet's contents
// onto the wire. That's always done at the NIC's line rate, and clearly should not be included in the RTT. eRPC uses
// per CPU core 'rdtsc()' for timestamps and, I believe, does not deal with seralization. Timely uses NIC provided
// timestamps. Fast NICs will require resolution sub-microsecond. Continuing from p540:
// 
//      For context, in a 10 Gbps network, serialization of 64 KB takes 51 µs, propagation may range from 10-100 µs,
//      and 1500 B of queuing takes 1.2 µs.
//
// The 64KB calculation is computed like this:
//
//      1/(10*1000*1000*1000) sec/bits * (64*1024*8) bits = .000052428sec ~= 52us
//
// Since Timely is assuming Tlow=10µs, Thigh=500µs it's important to account for this last term.

#include "timer.h"
#include <stdio.h>
#include <iostream>

namespace Reinvent {

class Timely {
public:
  // CONSTANTS
  const double d_alpha = 0.2;                       // EWMA smoothing factor (needs research)
  const double d_alphaResidue = (1.0 - d_alpha);
  const double d_beta = 0.80;                       // multiplicative decrease factor
  const double d_delta = 1250000.0;                 // additive rate increase 10 Mbit/sec expressed as byte-per-sec

  const double d_minRttUs = 10.0;                   // minimum RTT limit (us)
  const double d_maxRttUs = 1000.0;                 // maximum RTT limit (us)
  const double d_minRttClockCycles;                 // minimum RTT limit expressed in rdtsc() cycles

  const double d_cpuFreqGhz;                        // Caller's CPU clock rate (Ghz)
  const double d_maxLinkBandwidthBps;               // Maximum NIC bandwidth (bytes-per-sec)

  const double d_minRateBps = 500000.0;             // minimum transmit rate bytes/sec
  const double d_maxRateBps=d_maxLinkBandwidthBps;  // maximum transmit rate bytes/sec

  const double d_byteToGbits=8.0/(1000*1000*1000);  // convert from bytes to Gbits (Giga bits)

  const uint64_t d_minSampleUs = 1;                 // minimum elasped time (us) before state updated
  const uint64_t d_minSampleCpuCycles;              // d_minSampleCpuCycles is d_minSampleUs as cycles

private:
  double d_lineRateBps;                             // calculated TX rate (bytes-per-second)
  double d_prevRttUs;                               // last reported RTT (us)
  double d_weightedRttDiffUs;                       // weighted RTT difference (us)
  uint64_t d_prevCpuCycles;                         // Last time rdtsc() state was updated

public:
  // CREATORS
  Timely(double cpuFreqGhz, double maxLinkBandwidthBps, unsigned sessionCount);
    // Create a Timely object to estimate TX rate in bytes/sec with specified 'cpuFreqGhz', the frequency of caller's
    // CPU in Ghz, 'maxLinkBandwidthBps', and 'sessionCount' which is the number of running sessions not including
    // the session to be created and co-managed by this object. Behavior is defined provided the caller is pinned
    // to a HW core, 'maxLinkBandwidthBps' is at least 1 million bytes/sec, and '0.5<=cpuFreqGhz<=5.0'. 

  Timely() = delete;
    // Default constructor not provided

  Timely(const Timely& other) = delete;
    // Copy constructor not provided

  ~Timely() = default;
    // Destroy this object

  // ACCESSORS
  double rate() const;
    // Return the last estimated TX rate in bytes/sec. Note that, if called immediately following construction,
    // this returns the initial, estimated rate

  double rateAsGbps() const;
    // Exactly like 'rate' but expressed as Gbps (Giga bits per second)

  // MANIPULATORS
  double update(uint64_t nowClockCycles, double newRttUs);
    // Return new, estimated TX rate in bytes/sec where specified 'nowClockCycles' is the current or a very recent
    // 'rdtsc()' value and 'newRttUs' (units microseconds) is the most current RTT time. Behavior is defined provided
    // 'newRttUs>0' and 'nowClockCycles>d_prevCpuCycles'. This function is based on the correction in section 4.3 in
    // http://yibozhu.com/doc/ecndelay-conext16.pdf, and Timely author's code located her old UC Berkeley homepage
    // http://people.eecs.berkeley.edu/~radhika/timely-code-snippet.cc. Note both papers both elide details seen
    // in code snippet re: delta_factor (not to be confused with 'delta') and md_factor not to be confused by 'beta'.
    // Finally note eRPC code seems follow radhika's code too.

  // ASPECTS
  std::ostream& print(std::ostream& stream) const;
    // Print to specified 'stream' a human readable dump of this object's state returning 'stream'
};

// FREE OPERATORS
std::ostream& operator<<(std::ostream& stream, const Timely& object);
  // Print into specified 'stream' human readable dump of 'object' returning 'stream'

// INLINE DEFINITIONS
inline
Timely::Timely(double cpuFreqGhz, double maxLinkBandwidthBps, unsigned sessionCount)
: d_minRttClockCycles(d_minRttUs*1000.0*cpuFreqGhz)
, d_cpuFreqGhz(cpuFreqGhz)
, d_maxLinkBandwidthBps(maxLinkBandwidthBps)
, d_minSampleCpuCycles(static_cast<uint64_t>(d_minSampleUs*1000.0*cpuFreqGhz))
, d_lineRateBps(d_maxLinkBandwidthBps/(sessionCount+1.0))
, d_prevRttUs(d_minSampleUs)
, d_weightedRttDiffUs(0.0)
, d_prevCpuCycles(rdtsc())
{
  assert(d_alpha>0.0 && d_alpha<=1.0);
  assert(d_beta>0.0  && d_beta<=1.0);
  assert(d_delta>0.0 && d_delta>=1000000.0);
  assert(d_minRttUs>0.0 && d_minRttUs<d_maxRttUs);
  assert(d_maxRttUs>0.0 && d_maxRttUs>d_minRttUs);
  assert(cpuFreqGhz>=0.5 && cpuFreqGhz<=5.0);
  assert(maxLinkBandwidthBps>=1000000.0);
  assert(d_prevCpuCycles>0);
}

// ACCESSORS
inline
double Timely::rate() const {
  return d_lineRateBps;
}

inline
double Timely::rateAsGbps() const {
  return d_lineRateBps * d_byteToGbits;
}

// MANIPULATORS
inline
double Timely::update(uint64_t nowClockCycles, double newRttUs) {
  assert(nowClockCycles>d_prevCpuCycles);
  assert(newRttUs>0);

  // Measure absolute elapsed time from last update. These values come from rdtsc()
  const uint64_t absTimeDiff = nowClockCycles-d_prevCpuCycles;
  if (absTimeDiff<=d_minSampleCpuCycles) {
    return d_lineRateBps;
  }
  d_prevCpuCycles = nowClockCycles;

  // Calculate difference in current and previous RTT
  const double newRttDiff = newRttUs - d_prevRttUs;
  d_prevRttUs = newRttUs;

  // Calculate radhika's 'delta_factor'. This is a unitless number requiring
  // all terms use the same units
  double deltaFactor = static_cast<double>(absTimeDiff)/d_minRttClockCycles;
  deltaFactor = std::min(deltaFactor, 1.0);

  // Update weighted diff aka radhika's 'avg_rtt_diff_'
  d_weightedRttDiffUs = (d_alphaResidue*d_weightedRttDiffUs) + (d_alpha*newRttDiff);

  if (newRttUs < d_minRttUs) {
    d_lineRateBps += (deltaFactor*d_delta);
  } else if (newRttUs > d_maxRttUs) {
    const double multDecreaseFactor = deltaFactor * d_beta;
    d_lineRateBps = d_lineRateBps * (1 - multDecreaseFactor*(1.0-(d_maxRttUs/newRttUs)));
  } else {
    const double multDecreaseFactor = deltaFactor * d_beta;
    const double rttGradient = d_weightedRttDiffUs / d_minSampleUs;
    double weight(-1.0);
    if (rttGradient <= -0.25) {
      weight = 0.0;
    } else if (rttGradient >= 0.25) {
      weight = 1.0;
    } else {
      weight = 2*rttGradient + 0.5;
    }
    double error = (newRttUs-d_minRttUs) / d_minRttUs;
    d_lineRateBps = d_lineRateBps*(1.0 - (multDecreaseFactor*weight*error)) + deltaFactor*d_delta*(1-weight);
  }

  printf("raw new rate: %lf\n", d_lineRateBps);

  // Bound estimated rate above and below
  d_lineRateBps = std::min(d_lineRateBps, d_maxLinkBandwidthBps);
  return d_lineRateBps = std::max(d_lineRateBps, d_maxRateBps);
}

// ASPECTS
inline
std::ostream& Timely::print(std::ostream& stream) const {
  stream << "[" << std::endl;
  stream << "    rateGbps (last estimated rate)       : " << rateAsGbps()            << std::endl;
  stream << "    rateBps (last estimated rate)        : " << d_lineRateBps           << std::endl;
  stream << "    prevRttUs (last reported RTT us)     : " << d_prevRttUs             << std::endl;
  stream << "    alpha (EWMA smoothing factor)        : " << d_alpha                 << std::endl;
  stream << "    beta (multiplicative decrease factor): " << d_beta                  << std::endl;
  stream << "    delta (additive increase factor)     : " << d_delta                 << std::endl;
  stream << "    minRttUs (smallest RTT expected us)  : " << d_minRttUs              << std::endl;
  stream << "    minRttClockCycles (minRttUs as cycls): " << d_minRttClockCycles     << std::endl;
  stream << "    maxRttUs (largest  RTT expected us)  : " << d_maxRttUs              << std::endl;
  stream << "    cpuFreqGhz (CPU frequency Ghz)       : " << d_cpuFreqGhz            << std::endl;
  stream << "    NIC bandwidth (bytes/sec)            : " << d_maxLinkBandwidthBps   << std::endl;
  stream << "    minimum est rate (bytes/sec)         : " << d_minRateBps            << std::endl;
  stream << "    maximum est rate (bytes/sec)         : " << d_maxRateBps            << std::endl;
  stream << "    minimum sample time (us)             : " << d_minSampleUs           << std::endl;
  stream << "    minimum sample time (clock cycles)   : " << d_minSampleCpuCycles    << std::endl;
  stream << "]" << std::endl;
  return stream;
}

inline
std::ostream& operator<<(std::ostream& stream, const Timely& object) {
  return object.print(stream);
}

} // namespace Reinvent
