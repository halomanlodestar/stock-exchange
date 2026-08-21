#pragma once

#include <array>
#include <cstddef>
#include <memory>

namespace exchange::core {
template <typename T, std::size_t N> class MemoryPool {
  static_assert(N > 0, "Memory Pool capacity must be positive.");
  static_assert(sizeof(T) >= sizeof(void *),
                "Object must fit an intrusive free-list pointer.");

  struct alignas((alignof(T) > 64U) ? alignof(T) : 64U) Slot {
    std::array<std::byte, sizeof(T)> storage{};
  };

  struct FreeNode {
    FreeNode *next;
  };

public:
  MemoryPool() : slots(std::make_unique<Slot[]>(N)) { initialize_free_list(); }

  MemoryPool(const MemoryPool &) = delete;
  MemoryPool &operator=(const MemoryPool &) = delete;
  MemoryPool(MemoryPool &&) = delete;
  MemoryPool &operator=(MemoryPool &&) = delete;

  T *allocate() {
    if (listHead == nullptr) [[unlikely]] {
      return nullptr;
    }

    FreeNode *node = reinterpret_cast<T *>(listHead);
    listHead = listHead->next;

    return node;
  }

  void deallocate(T *object) {
    if (object == nullptr) [[unlikely]] {
      return;
    }

    std::destroy_at(object);

    FreeNode *freeNode = reinterpret_cast<FreeNode *>(object);

    freeNode->next = listHead;
    listHead = freeNode;
  }

private:
  std::unique_ptr<Slot[]> slots;
  FreeNode &listHead{nullptr};

  void initialize_free_list() {

    for (std::size_t i = 0; i < N; ++i) {
      auto *node = reinterpret_cast<FreeNode *>(slots[i].storage.data());
      node->next = listHead;
      listHead = node;
    }
  }
};
} // namespace exchange::core
