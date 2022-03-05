#include <dpdk/reinvent_dpdk_lcore.h>

namespace Reinvent {

std::ostream& Dpdk::LCORE::print(std::ostream& stream) const {
  stream << "{"
         << "\"id\":"       << d_id
         << ", \"vcpu\":"     << d_vcpuId
         << ", \"role\":[";

  for (unsigned i=0; i<d_role.size(); ++i) {
    switch (d_role[i]) {
      case Dpdk::LCORE::e_LCORE_RXQ:
        stream << "\"RXQ\"";
        break; 
      case Dpdk::LCORE::e_LCORE_TXQ:
        stream << "\"TXQ\"";
        break; 
      default:
        stream << "\"*unknown*\"";
        break; 
    }
    if (i+1!=d_role.size()) {
      stream << ",";
    }
    stream << "]";
  }

  stream << "}";

  return stream;
}

} // namespace Reinvent
