#include <dpdk/reinvent_dpdk_rxq.h>

namespace Reinvent {

std::ostream& Dpdk::RXQ::print(std::ostream& stream) const {
  stream << "{ "
         << "\"id\":"                 << d_id
         << ", \"vcpuId\":"           << d_vcpuId
         << ", \"lcoreId\":"          << d_lcoreId
         << ", \"memzone\":\""        << d_memzone        << "\""
         << ", \"mempool\":\""        << d_mempool        << "\""
         << ", \"ringSize\":"         << d_ringSize
         << ", \"mempoolPolicy\":\""  << d_mempoolPolicy  << "\""
         << "}";
  return stream;
}

} // namespace Reinvent
