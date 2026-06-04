#ifndef STRICTLY_WORSE_VECTOR_HPP
#define STRICTLY_WORSE_VECTOR_HPP

#include <cstddef>
#include <cstring>
#include <memory>
#include <new>
#include <ranges>
#include <type_traits>
#include <utility>

namespace StrictlyWorse {

template <typename T> class Vector {
public:
  Vector() = default;
  ~Vector() {
    for (auto const idx : std::views::iota(0UZ, size_)) {
      std::destroy_at(&data_[idx]);
    }
    operator delete(data_, static_cast<std::align_val_t>(alignof(T)));
  }

  auto push_back(T const &value) -> void {
    if (data_ == nullptr || size_ == capacity_) {
      reallocate();
    }

    std::construct_at(data_ + size_++, value);
  }

  [[nodiscard]] constexpr auto is_empty() const { return size_ == 0UZ; }
  [[nodiscard]] constexpr auto data() const { return data_; }
  [[nodiscard]] constexpr auto size() const { return size_; }
  [[nodiscard]] constexpr auto capacity() const { return capacity_; }

private:
  T *data_{};
  std::size_t capacity_{};
  std::size_t size_{};

  auto reallocate() -> void {
    // If data_ is nullptr, simply add one element.
    if (data_ == nullptr) {
      data_ = static_cast<T *>(operator new(
          sizeof(T), static_cast<std::align_val_t>(alignof(T))));
      capacity_ = 1UZ;
      return;
    }

    std::size_t new_capacity = capacity_ * 2;

    auto *new_data{static_cast<T *>(operator new(
        new_capacity * sizeof(T), static_cast<std::align_val_t>(alignof(T))))};

    // Trivially copyable types can be swapped by a simple memcpy, no move or
    // iteration required.
    if constexpr (std::is_trivially_copyable_v<T>) {
      std::memcpy(new_data, data_, size_ * sizeof(T));
      operator delete(data_, static_cast<std::align_val_t>(alignof(T)));
      data_ = new_data;
      return;
    }

    // If T's move constructor can't throw, destroy the old objects in the same
    // iteration.
    if constexpr (std::is_nothrow_move_constructible_v<T>) {
      for (auto const idx : std::views::iota(0UZ, size_)) {
        std::construct_at(new_data + idx, std::move(data_[idx]));
        std::destroy_at(&data_[idx]);
      }

      operator delete(data_, static_cast<std::align_val_t>(alignof(T)));
      data_ = new_data;
      return;
    }

    // If T's copy constructor can't throw, destroy the old objects in the same
    // iteration.
    if constexpr (std::is_nothrow_copy_constructible_v<T>) {
      for (auto const idx : std::views::iota(0UZ, size_)) {
        std::construct_at(new_data + idx, data_[idx]);
        std::destroy_at(&data_[idx]);
      }

      operator delete(data_, static_cast<std::align_val_t>(alignof(T)));
      data_ = new_data;
      return;
    }

    // If moving or copying a T can throw, maintain the old collection and only
    // commit the new memory and capacity once all new objects have been
    // successfully constructed.  Track successful construction so that they can
    // be destroyed prior to freeing the memory.

    std::size_t elements_constructed{};

    try {
      for (auto const idx : std::views::iota(0UZ, size_)) {
        std::construct_at(new_data + idx, data_[idx]);
        ++elements_constructed;
      }

      for (auto const idx : std::views::iota(0UZ, size_)) {
        std::destroy_at(&data_[idx]);
      }

      operator delete(data_, static_cast<std::align_val_t>(alignof(T)));
      data_ = new_data;
      capacity_ = new_capacity;
      return;
    } catch (...) {
      // Exception encountered during migration of objects from old to new
      // storage, deconstruct the successful copies and free the newly allocated
      // memory to clean up.  Do not commit data_ or capacity_ updates to
      // maintain strong exception safety guarantees.

      for (auto const idx : std::views::iota(0UZ, elements_constructed)) {
        std::destroy_at(&new_data[idx]);
      }

      operator delete(new_data, static_cast<std::align_val_t>(alignof(T)));
      throw;
    }
  }
};

} // namespace StrictlyWorse
#endif
