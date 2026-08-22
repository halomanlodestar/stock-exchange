#include "core/types.h"
#include <chrono>

namespace exchange::core {

class LogicalClock {
public:
  Timestamp next() noexcept { return ++time; }

  Timestamp current() const noexcept { return time; }

  void reset() noexcept { time = 0; }

private:
  Timestamp time{0};
};

[[nodiscard]] inline Timestamp steady_timestamp_now() {
  const auto now = std::chrono::steady_clock::now().time_since_epoch();

  return static_cast<Timestamp>(std::chrono::nanoseconds(now).count());
}

[[nodiscard]] inline Timestamp now_ms() { return steady_timestamp_now(); }

} // namespace exchange::core