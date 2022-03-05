#include <dpdk/reinvent_dpdk_udproute.h>

namespace Reinvent {

std::ostream& Dpdk::UDPRoute::print(std::ostream& stream) const {
  stream << "{ "
         << "\"sourceMac\":\""            << d_srcMac << "\""
         << ", \"destinationMac\":\""     << d_dstMac << "\""
         << ", \"sourceIPV4\":\""         << d_srcIp << "\""
         << ", \"destinationIPV4\":\""    << d_dstIp << "\""
         << ", \"sourcePort\":"           << d_srcPort
         << ", \"destinationPort\":"      << d_dstPort;
  stream << "}";

  return stream;
}

} // namespace Reinvent
