module;
#include <memory>
export module swtl_vector;

import std;

namespace swtl {

export template <typename T> class VectorIterator {
public:
  using iterator_category = std::contiguous_iterator_tag;
  using value_type = std::remove_cv_t<T>;
  using difference_type = std::ptrdiff_t;
  using pointer = T *;
  using reference = T &;

  constexpr VectorIterator() = default;
  constexpr explicit VectorIterator(pointer ptr) : ptr_{ptr} {}

  [[nodiscard]] constexpr auto operator*() const -> reference { return *ptr_; }
  [[nodiscard]] constexpr auto operator->() const -> pointer { return ptr_; }
  [[nodiscard]] constexpr auto operator[](difference_type idx) const noexcept
      -> reference {
    return ptr_[idx];
  }

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

  [[nodiscard]] constexpr friend auto
  operator+(VectorIterator const &lhs, difference_type distance) noexcept
      -> VectorIterator {
    return VectorIterator{lhs.ptr_ + distance};
  }

  [[nodiscard]] constexpr friend auto
  operator+(difference_type distance, VectorIterator const &rhs) noexcept
      -> VectorIterator {
    return VectorIterator{rhs + distance};
  }

  [[nodiscard]] constexpr friend auto
  operator-(VectorIterator const &lhs, difference_type distance) noexcept
      -> VectorIterator {
    return VectorIterator{lhs.ptr_ - distance};
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

// Ensures that the iterator meets the
// requirements for the appropriate iterator tag.
static_assert(std::contiguous_iterator<VectorIterator<int>>);
static_assert(std::contiguous_iterator<VectorIterator<int const>>);

export template <typename T, typename Allocator = std::allocator<T>>
class Vector {
public:
  using value_type = std::remove_cv_t<T>;
  using allocator_type = Allocator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type &;
  using const_reference = value_type const &;
  using pointer = std::allocator_traits<Allocator>::pointer;
  using const_pointer = std::allocator_traits<Allocator>::const_pointer;
  using iterator = VectorIterator<T>;
  using const_iterator = VectorIterator<T const>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  Vector() = default;

  Vector(Vector const &other) {
    data_ =
        std::allocator_traits<Allocator>::allocate(allocator_, other.capacity_);
    try {
      std::uninitialized_copy(other.begin(), other.end(),
                              VectorIterator{data_});
      capacity_ = other.capacity_;
      size_ = other.size_;
    } catch (...) {
      std::allocator_traits<Allocator>::deallocate(allocator_, data_,
                                                   other.capacity_);
      throw;
    }
  }

  Vector &operator=(Vector const &other) {
    if (this == std::addressof(other)) {
      return *this;
    }

    if constexpr (std::allocator_traits<Allocator>::
                      propagate_on_container_copy_assignment::value) {
      if (allocator_ != other.allocator_) {
        std::ranges::destroy(begin(), end());
        std::allocator_traits<Allocator>::deallocate(allocator_, data_,
                                                     capacity_);
        data_ = nullptr;
        size_ = 0UZ;
        capacity_ = 0UZ;
      }
      allocator_ = other.allocator_;
    }

    Vector temp{other};
    swap(temp);
    return *this;
  }

  Vector(Vector &&other) noexcept
      : allocator_{std::move(other.allocator_)}, data_{other.data_},
        capacity_{other.capacity_}, size_{other.size_} {
    other.data_ = nullptr;
    other.capacity_ = 0UZ;
    other.size_ = 0UZ;
  }

  Vector &operator=(Vector &&other) noexcept(
      std::allocator_traits<
          Allocator>::propagate_on_container_move_assignment::value ||
      std::allocator_traits<Allocator>::is_always_equal::value) {
    if (this == std::addressof(other)) {
      return *this;
    }
    if constexpr (std::allocator_traits<Allocator>::
                      propagate_on_container_move_assignment::value) {
      std::ranges::destroy(begin(), end());
      std::allocator_traits<Allocator>::deallocate(allocator_, data_,
                                                   capacity_);
      allocator_ = std::move(other.allocator_);
      data_ = other.data_;
      capacity_ = other.capacity_;
      size_ = other.size_;
      other.data_ = nullptr;
      other.capacity_ = 0UZ;
      other.size_ = 0UZ;
    } else if (allocator_ == other.allocator_) {
      std::ranges::destroy(begin(), end());
      std::allocator_traits<Allocator>::deallocate(allocator_, data_,
                                                   capacity_);
      data_ = other.data_;
      capacity_ = other.capacity_;
      size_ = other.size_;
      other.data_ = nullptr;
      other.capacity_ = 0UZ;
      other.size_ = 0UZ;
    } else {
      auto *new_data{std::allocator_traits<Allocator>::allocate(
          allocator_, other.capacity_)};
      try {
        std::uninitialized_move(other.begin(), other.end(),
                                VectorIterator{new_data});
        std::ranges::destroy(begin(), end());
        std::allocator_traits<Allocator>::deallocate(allocator_, data_,
                                                     capacity_);
        data_ = new_data;
        new_data = nullptr;
        size_ = other.size_;
        capacity_ = other.capacity_;
      } catch (...) {
        std::allocator_traits<Allocator>::deallocate(allocator_, new_data,
                                                     other.capacity_);
        throw;
      }
    }
    return *this;
  }

  constexpr ~Vector() {
    std::ranges::destroy(VectorIterator{data_}, VectorIterator{data_ + size_});
    if (data_ != nullptr) {
      std::allocator_traits<Allocator>::deallocate(allocator_, data_,
                                                   capacity_);
    }
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
  constexpr auto swap(Vector &other) noexcept -> void {
    if constexpr (std::allocator_traits<
                      Allocator>::propagate_on_container_swap::value) {
      std::swap(allocator_, other.allocator_);
    }
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
  }

  constexpr auto push_back(T const &value) -> void {
    if (size_ == capacity_) {
      std::size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;

      auto new_data{
          std::allocator_traits<Allocator>::allocate(allocator_, new_capacity)};

      if constexpr (std::is_nothrow_move_constructible_v<T>) {
        std::ranges::uninitialized_move(
            VectorIterator{data_}, VectorIterator{data_ + size_},
            VectorIterator{new_data}, VectorIterator{new_data + size_});
      } else if constexpr (std::is_nothrow_copy_constructible_v<T>) {
        std::ranges::uninitialized_copy(
            VectorIterator{data_}, VectorIterator{data_ + size_},
            VectorIterator{new_data}, VectorIterator{new_data + size_});
      } else {
        try {
          std::ranges::uninitialized_copy(
              VectorIterator{data_}, VectorIterator{data_ + size_},
              VectorIterator{new_data}, VectorIterator{new_data + size_});
        } catch (...) {
          // The ununitialized_copy function takes care of destroying the new
          // objects that it created during attempted migration, just have to
          // free the memory before rethrowing.
          std::allocator_traits<Allocator>::deallocate(allocator_, new_data,
                                                       new_capacity);
          throw;
        }
      }

      // The uninitialized_ functions don't destroy objects unless the copy
      // fails, and even then it's only the objects that were created prior to
      // the error - not the originals.  So on successful reallocation:
      // destroy the old objects, give the memory back to the allocator, and
      // update our internals.
      std::ranges::destroy(VectorIterator{data_},
                           VectorIterator{data_ + size_});
      if (data_ != nullptr) {
        std::allocator_traits<Allocator>::deallocate(allocator_, data_,
                                                     capacity_);
      }
      data_ = new_data;
      capacity_ = new_capacity;
    }

    std::allocator_traits<Allocator>::construct(allocator_, data_ + size_++,
                                                value);
  }

  constexpr friend auto swap(Vector &lhs, Vector &rhs) noexcept -> void {
    lhs.swap(rhs);
  }

private:
  [[no_unique_address]] Allocator allocator_;
  T *data_{};
  std::size_t capacity_{};
  std::size_t size_{};
};

} // namespace swtl
