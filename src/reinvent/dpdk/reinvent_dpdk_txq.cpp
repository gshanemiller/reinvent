#include <dpdk/reinvent_dpdk_txq.h>

namespace Reinvent {

std::ostream& Dpdk::TXQ::print(std::ostream& stream) const {
  stream << "{"
         << " \"id\":"                << d_id
         << ", \"vcpuId\":"           << d_vcpuId
         << ", \"lcoreId\":"          << d_lcoreId
         << ", \"memzone\":\""        << d_memzone        << "\""
         << ", \"mempool\":\""        << d_mempool        << "\""
         << ", \"mempoolPolicy\":\""  << d_mempoolPolicy  << "\""
         << ", \"ringSize\":"         << d_ringSize                                                                     
         << ", \"defaultFlow\":"      << d_defaultFlow
         << "}";
  return stream;
}

} // namespace Reinvent
