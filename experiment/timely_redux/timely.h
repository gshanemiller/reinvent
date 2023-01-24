#pragma once

#include "timer.h"
#include <stdio.h>
#include <iostream>

namespace Reinvent {

class Timely {
  // CONSTANTS
  const double d_alpha = 0.46;                  // EWMA smoothing factor
  const double d_alphaResidue = (1.0 - d_alpha);
  const double d_beta = 0.26;                   // multiplicative decrease factor

  const double d_delta = (5.0*1000*1000);       // how much rate is additively increased
  const double d_minRttUs = 50.0;               // minimum RTT limit (us)
  const double d_maxRttUs = 1000.0;             // maximum RTT limit (us)

  const double d_cpuFreqGhz;                    // Caller's CPU clock rate (Ghz)
  const double d_maxLinkBandwidthBps;           // Maximum NIC bandwidth (bps)

  double d_newLineRateBps;                      // New, calculated TX rate (bps)
  double d_prevRttUs;                           // Last RTT (us)
  double d_weightedRttDiffUs;                   // Weighted RTT difference

public:
  // CREATORS
  Timely(double cpuFreqGhz, double maxLinkBandwidthBps, unsigned sessionCount);
    // Create a Timely object to estimate TX rate in bps with specified 'cpuFreqGhz', the frequency of caller's
    // CPU in Ghz, 'maxLinkBandwidthBps', and 'sessionCount' which is the number of running sessions not including
    // the session to be created and co-managed by this object. Behavior is defined provided the caller is pinned
    // to a HW core, 'maxLinkBandwidthBps' is at least 1 million bps, and '0.5<=cpuFreqGhz<=5.0'. 

  Timely() = delete;
    // Default constructor not provided

  Timely(const Timely& other) = delete;
    // Copy constructor not provided

  ~Timely() = default;
    // Destroy this object

  // ACCESSORS
  double lastRate() const;
    // Return the last estimated TX rate in bps. Note that, if called immediately following construction, this
    // returns the initial estimated rate

  // MANIPULATORS
  double newRate(double newRttUs);
    // Estimate the new TX rate in bps based on the specified current 'newRttUs' in us. This function is based on
    // Zhu et al. http://yibozhu.com/doc/ecndelay-conext16.pdf correction to the Timely algorithm in section 4.3

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
: d_cpuFreqGhz(cpuFreqGhz)
, d_maxLinkBandwidthBps(maxLinkBandwidthBps)
, d_newLineRateBps(d_maxLinkBandwidthBps/(sessionCount+1.0))
, d_prevRttUs(d_minRttUs)
, d_weightedRttDiffUs(0.0)
{
  assert(d_alpha>0.0 && d_alpha<=1.0);
  assert(d_beta>0.0  && d_beta<=1.0);
  assert(d_delta>0.0 && d_delta>=1000000.0);
  assert(d_minRttUs>0.0 && d_minRttUs<d_maxRttUs);
  assert(d_maxRttUs>0.0 && d_maxRttUs>d_minRttUs);
  assert(cpuFreqGhz>=0.5 && cpuFreqGhz<=5.0);
  assert(maxLinkBandwidthBps>=1000000.0);
}

// ACCESSORS
inline
double Timely::lastRate() const {
  return d_newLineRateBps;
}

// MANIPULATORS
inline
double Timely::newRate(double newRttUs) {
  const double newRttDiff = newRttUs - d_prevRttUs;
  d_prevRttUs = newRttUs;
  d_weightedRttDiffUs = (d_alphaResidue*d_weightedRttDiffUs) + (d_alpha*newRttDiff);
  double rttGradient = d_weightedRttDiffUs / d_minRttUs;

  if (newRttUs < d_minRttUs) {
    d_newLineRateBps += d_delta;
  } else if (newRttUs > d_maxRttUs) {
    d_newLineRateBps = d_newLineRateBps * (1 - d_beta*(1.0-(d_maxRttUs/newRttUs)));
  } else {
    double weight(-1.0);
    if (rttGradient <= -0.25) {
      weight = 0.0;
    } else if (rttGradient >= 0.25) {
      weight = 1.0;
    } else {
      weight = 2*rttGradient + 0.5;
    }
    double error  = (newRttUs-d_minRttUs) / d_minRttUs;
    d_newLineRateBps = d_delta*(1-weight) + d_newLineRateBps*(1.0 - (d_beta*weight*error));
  }

  return d_newLineRateBps;
}

// ASPECTS
inline
std::ostream& Timely::print(std::ostream& stream) const {
  stream << "[" << std::endl;
  stream << "    newLineRateBps (last est rate bps)   : " << d_maxLinkBandwidthBps   << std::endl;
  stream << "    prevRttUs (last reported RTT us)     : " << d_prevRttUs             << std::endl;
  stream << "    beta (multiplicative decrease factor): " << d_beta                  << std::endl;
  stream << "    alpha (EWMA smoothing factor)        : " << d_alpha                 << std::endl;
  stream << "    delta (additive increase factor)     : " << d_delta                 << std::endl;
  stream << "    minRttUs (smallest RTT expected us)  : " << d_minRttUs              << std::endl;
  stream << "    maxRttUs (largest  RTT expected us)  : " << d_maxRttUs              << std::endl;
  stream << "    cpuFreqGhz (CPU frequency Ghz)       : " << d_cpuFreqGhz            << std::endl;
  stream << "    maxLinkBandwidthBps (max NIC bw bps) : " << d_maxLinkBandwidthBps   << std::endl;
  stream << "]" << std::endl;

  return stream;
}

inline
std::ostream& operator<<(std::ostream& stream, const Timely& object) {
  return object.print(stream);
}

} // namespace Reinvent
