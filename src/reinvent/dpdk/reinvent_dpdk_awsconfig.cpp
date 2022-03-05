#include <dpdk/reinvent_dpdk_awsconfig.h>

#include <util/reinvent_util_errno.h>

namespace Reinvent {                                                                                                    

std::ostream& Dpdk::AWSEnaConfig::print(std::ostream& stream) const {                                                  
  stream << "{";
 
  stream << "\"dpdkArgs\" : [";
  for (unsigned i=0; i<d_dpdkArgs.size(); i++) {
    stream << "\"" << d_dpdkArgs[i] << "\"";
    if ((i+1)!=d_dpdkArgs.size()) {
      stream << ",";
    }
  }
  stream << "],";


  stream << "\"deviceId\":"         << d_deviceId         << ","
         << "\"pciId\":\""          << d_pciId            << "\","
         << "\"numaNode\":"         << d_numaNode         << ","
         << "\"rxqCount\":"         << d_rxqCount         << ","
         << "\"txqCount\":"         << d_txqCount         << ","
         << "\"rxqThreadCount\":"   << d_rxqThreadCount   << ","
         << "\"txqThreadCount\":"   << d_txqThreadCount   << ","
         << "\"rxqPolicy\":\""      << d_rxqPolicy        << "\","
         << "\"txqPolicy\":\""      << d_txqPolicy        << "\",";

  stream << "\"rxqCandidateVcpuList\":[";
  for (unsigned i=0; i<d_rxqCandidateVcpuList.size(); i++) {
    stream << d_rxqCandidateVcpuList[i];
    if ((i+1)!=d_rxqCandidateVcpuList.size()) {
      stream << ",";
    }
  }
  stream << "],";

  stream << "\"txqCandidateVcpuList\":[";
  for (unsigned i=0; i<d_txqCandidateVcpuList.size(); i++) {
    stream << d_txqCandidateVcpuList[i];
    if ((i+1)!=d_txqCandidateVcpuList.size()) {
      stream << ",";
    }
  }
  stream << "],";

  stream << "\"mtu\":"              << d_mtu              << ","
         << "\"memzoneMask\":"      << d_memzoneMask      << ","
         << "\"memzoneReserveKb\":" << d_memzoneReserveKb << ","
         << "\"memzoneName\":\""    << d_memzoneName      << "\","
         << "\"mempoolPolicy\":\""  << d_mempoolPolicy    << "\",";

  stream << "\"rxq\":[";
  for (unsigned i=0; i<d_rxq.size(); i++) {
    stream << d_rxq[i];
    if ((i+1)!=d_rxq.size()) {
      stream << ",";
    }
  }
  stream << "],";

  stream << "\"txq\":[";
  for (unsigned i=0; i<d_txq.size(); i++) {
    stream << d_txq[i];
    if ((i+1)!=d_txq.size()) {
      stream << ",";
    }
  }
  stream << "],";

  stream << "\"lcore\":[";
  for (unsigned i=0; i<d_lcore.size(); i++) {
    stream << d_lcore[i];
    if ((i+1)!=d_lcore.size()) {
      stream << ",";
    }
  }
  stream << "],";

  stream << "\"vcpu\":[";
  for (unsigned i=0; i<d_vcpu.size(); i++) {
    stream << d_vcpu[i];
    if ((i+1)!=d_vcpu.size()) {
      stream << ",";
    }
  }
  stream << "],";

  stream << "\"linkSpeed\":"        << d_linkSpeed        << ","
         << "\"rxMqMask\":"         << d_rxMqMask         << ","
         << "\"txMqMask\":"         << d_txMqMask         << ","
         << "\"rxOffloadMask\":"    << d_rxOffloadMask    << ","
         << "\"txOffloadMask\":"    << d_txOffloadMask    << ","
         << "\"defaultTxFlow\":"    << d_defaultTxFlow;

  stream << "}";

  return stream;
}

} // namespace Reinvent
