export module swtl_vector;

import std;

namespace swtl {

export template <typename T> class VectorIterator {
public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = std::remove_cv_t<T>;
  using difference_type = std::ptrdiff_t;
  using pointer = T *;
  using reference = T &;

  constexpr VectorIterator() = default;
  constexpr explicit VectorIterator(pointer ptr) : ptr_{ptr} {}

  [[nodiscard]] constexpr auto operator*() const -> reference { return *ptr_; }
  [[nodiscard]] constexpr auto operator->() const -> pointer { return ptr_; }

  constexpr auto operator++() noexcept -> VectorIterator & {
    ++ptr_;
    return *this;
  }

  constexpr auto operator++(int) noexcept -> VectorIterator {
    auto temp{*this};
    ++ptr_;
    return temp;
  }

  constexpr auto operator--() noexcept -> VectorIterator & {
    --ptr_;
    return *this;
  }

  constexpr auto operator--(int) noexcept -> VectorIterator {
    auto temp{*this};
    --ptr_;
    return temp;
  }

  constexpr auto operator+=(difference_type distance) noexcept
      -> VectorIterator & {
    ptr_ += distance;
    return *this;
  }

  constexpr auto operator-=(difference_type distance) noexcept
      -> VectorIterator & {
    ptr_ -= distance;
    return *this;
  }

  [[nodiscard]] constexpr auto operator[](difference_type idx) const noexcept
      -> reference {
    return ptr_[idx];
  }

  [[nodiscard]] constexpr friend auto
  operator+(VectorIterator const &lhs, difference_type distance) noexcept
      -> VectorIterator {
    return lhs.ptr_ + distance;
  }

  [[nodiscard]] constexpr friend auto
  operator+(difference_type distance, VectorIterator const &rhs) noexcept
      -> VectorIterator {
    return rhs + distance;
  }

  [[nodiscard]] constexpr friend auto
  operator-(VectorIterator const &lhs, difference_type distance) noexcept
      -> VectorIterator {
    return lhs.ptr_ - distance;
  }

  [[nodiscard]] constexpr friend auto
  operator-(difference_type distance, VectorIterator const &rhs) noexcept
      -> VectorIterator {
    return rhs - distance;
  }

  [[nodiscard]] constexpr friend auto
  operator-(VectorIterator const &lhs, VectorIterator const &rhs) noexcept
      -> difference_type {
    return lhs.ptr_ - rhs.ptr_;
  }

  [[nodiscard]] constexpr friend auto
  operator<=>(VectorIterator const &lhs,
              VectorIterator const &rhs) noexcept = default;

private:
  pointer ptr_{};
};

static_assert(std::random_access_iterator<VectorIterator<int>>);
static_assert(std::random_access_iterator<VectorIterator<int const>>);

export template <typename T> class Vector {
public:
  using iterator = VectorIterator<T>;
  using const_iterator = VectorIterator<T const>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  Vector() = default;
  ~Vector() {
    for (auto const idx : std::views::iota(0UZ, size_)) {
      std::destroy_at(&data_[idx]);
    }
    operator delete(data_, static_cast<std::align_val_t>(alignof(T)));
  }

  auto push_back(T const &value) -> void {
    if (size_ == capacity_) {
      reallocate();
    }

    std::construct_at(data_ + size_++, value);
  }

  [[nodiscard]] constexpr auto begin() noexcept -> iterator {
    return iterator{data_};
  }
  [[nodiscard]] constexpr auto end() noexcept -> iterator {
    return iterator{data_ + size_};
  }
  [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
    return const_iterator{data_};
  }
  [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
    return const_iterator{data_ + size_};
  }
  [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator {
    return begin();
  }
  [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator {
    return end();
  }
  [[nodiscard]] constexpr auto rbegin() noexcept -> reverse_iterator {
    return reverse_iterator{end()};
  }
  [[nodiscard]] constexpr auto rend() noexcept -> reverse_iterator {
    return reverse_iterator{begin()};
  }
  [[nodiscard]] constexpr auto rbegin() const noexcept
      -> const_reverse_iterator {
    return reverse_iterator{end()};
  }
  [[nodiscard]] constexpr auto rend() const noexcept -> const_reverse_iterator {
    return reverse_iterator{begin()};
  }
  [[nodiscard]] constexpr auto crbegin() const noexcept
      -> const_reverse_iterator {
    return rbegin();
  }
  [[nodiscard]] constexpr auto crend() const noexcept
      -> const_reverse_iterator {
    return rend();
  }

  [[nodiscard]] constexpr auto data() const noexcept { return data_; }
  [[nodiscard]] constexpr auto size() const { return size_; }
  [[nodiscard]] constexpr auto capacity() const noexcept { return capacity_; }
  [[nodiscard]] constexpr auto is_empty() const noexcept {
    return size_ == 0UZ;
  }

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
      capacity_ = new_capacity;
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
      capacity_ = new_capacity;
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
      capacity_ = new_capacity;
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

} // namespace swtl
