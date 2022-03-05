#include <dpdk/reinvent_dpdk_vcpu.h>

namespace Reinvent {

std::ostream& Dpdk::VCPU::print(std::ostream& stream) const {
  stream << "{ \"id\":"             << d_id
         << ", \"core\":"           << d_core
         << ", \"socket\":"         << d_socket
         << ", \"numaNode\":"       << d_numaNode
         << ", \"softEnabled\":"    << d_softEnabled;
  stream << "}";

  return stream;
}

} // namespace Reinvent
