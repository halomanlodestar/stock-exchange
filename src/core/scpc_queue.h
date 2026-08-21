#pragma once

#include <array>
#include <atomic>
#include <cstdalign>
#include <cstdint>
#include <type_traits>

namespace exchange::core {
template <typename T, std::size_t N> class SPSCQueue {
  static_assert(N > 1, "Queue requires atleast 2 slots");
  static_assert((N & (N - 1)) == 0U, "Queue size must be a power of 2");
  static_assert(std::is_trivially_copyable_v<T>,
                "Type T must be trivially copyable");

  struct alignas(128U) Cursor {
    std::atomic<std::uint64_t> value{0};
  };

  bool push(const T &item) noexcept {

    std::uint64_t tail = this->tail.value.load(std::memory_order_relaxed);
    std::uint64_t head = this->head.value.load(std::memory_order_acquire);

    if ((tail - head) == N) [[unlikely]] {
      return false;
    }

    buffer[tail & MASK] = item;
    this->tail.value.store(tail + 1U, std::memory_order_release);

    return true;
  }

  bool pop(T &value) {

    std::uint64_t head = this->head.value.load(std::memory_order_relaxed);
    std::uint64_t tail = this->tail.value.load(std::memory_order_acquire);

    if (head == tail) [[unlikely]] {
      return false;
    }

    value = buffer[head & MASK];
    this->head.value.store(head + 1U, std::memory_order_release);

    return true;
  }

  [[nodiscard]] bool empty() const noexcept {
    return this->head.value.load(std::memory_order_acquire) ==
           this->tail.value.load(std::memory_order_acquire);
  }

private:
  static constexpr std::size_t MASK = N - 1U;
  std::array<T, N> buffer;

  Cursor head;
  Cursor tail;
};
}; // namespace exchange::core