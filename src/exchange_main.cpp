#include "core/memory_pool.h"
#include "core/scpc_queue.h"
#include "core/types.h"

using namespace std;

int main() {
  exchange::core::MemoryPool<exchange::core::Price, 1> memPool;
  exchange::core::SPSCQueue<std::int64_t, 2>();
}