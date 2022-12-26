#include <dpdk/reinvent_dpdk_ipv4route.h>

namespace Reinvent {

std::ostream& Dpdk::IPV4Route::print(std::ostream& stream) const {
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
