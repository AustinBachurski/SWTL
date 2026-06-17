export module swtl_vector;

import std;
import swtl_format;
import swtl_memory;

namespace swtl {

export template <typename T> class VectorIterator {
public:
  using iterator_category = std::contiguous_iterator_tag;
  using value_type = std::remove_cv_t<T>;
  using difference_type = std::ptrdiff_t;
  using pointer = T *;
  using reference = T &;

  template <typename U> friend class VectorIterator;

  template <typename U>
    requires std::is_const_v<T> && std::same_as<U, std::remove_const_t<T>>
  constexpr VectorIterator(VectorIterator<U> const &other) : ptr_{other.ptr_} {}

  constexpr VectorIterator() = default;
  constexpr explicit VectorIterator(pointer ptr) : ptr_{ptr} {}

  [[nodiscard]] constexpr auto operator*() const noexcept -> reference {
    return *ptr_;
  }

  [[nodiscard]] constexpr auto operator->() const noexcept -> pointer {
    return ptr_;
  }

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
  // ** MEMBER TYPES **
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

  // ** CONSTRUCTORS **
  Vector() = default;

  explicit Vector(size_type count) {
    reserve(count);

    for (auto const _ : std::views::iota(0UZ, count)) {
      emplace_back();
    }
  }

  Vector(std::initializer_list<T> const &init_list)
      : data_{std::allocator_traits<Allocator>::allocate(allocator_,
                                                         init_list.size())},
        capacity_{init_list.size()}, size_{init_list.size()} {
    allocator_aware::uninitialized_copy_range(allocator_, init_list.begin(),
                                              init_list.end(), begin());
  }

  template <std::input_iterator InputIterator,
            std::sentinel_for<InputIterator> Sentinel>
  Vector(InputIterator in_begin, Sentinel in_end) {
    auto const distance{std::ranges::distance(in_begin, in_end)};

    if (distance < 0) {
      throw std::invalid_argument(
          "Vector constructor: 'in_end' must be reachable from 'in_begin'");
    }

    auto const count{static_cast<size_type>(distance)};
    reserve(count);

    allocator_aware::uninitialized_copy_range(allocator_, in_begin, in_end,
                                              begin());
    size_ = count;
  }

  // ** SPECIAL MEMBER FUNCTIONS **
  Vector(Vector const &other)
      : allocator_{
            std::allocator_traits<Allocator>::
                select_on_container_copy_construction(other.allocator_)} {
    data_ =
        std::allocator_traits<Allocator>::allocate(allocator_, other.capacity_);
    try {
      allocator_aware::uninitialized_copy_range(
          allocator_, other.begin(), other.end(), VectorIterator{data_});
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
        allocator_aware::destroy_range(allocator_, begin(), end());
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
      // Allocator can be moved, with the container.  Move over the allocator,
      // pointer, and invariants.

      allocator_aware::destroy_range(allocator_, begin(), end());
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
      // Allocator can't be moved over, but the destination and source
      // allocators compare equal.  Ignore the allocator, move over the pointer
      // and invariants.

      allocator_aware::destroy_range(allocator_, begin(), end());
      std::allocator_traits<Allocator>::deallocate(allocator_, data_,
                                                   capacity_);
      data_ = other.data_;
      capacity_ = other.capacity_;
      size_ = other.size_;
      other.data_ = nullptr;
      other.capacity_ = 0UZ;
      other.size_ = 0UZ;
    } else {
      // Worst case scenario, can't transfer memory at all since the allocator
      // can't be moved and is not equal to the existing one.  Allocate new
      // memory with the current allocator and move the elements over.  On
      // success, destroy the destination container's existing objects, free the
      // memory, and swap the pointer from the newly allocated memory into
      // `data_`, then copy the size and capacity from the source.
      //
      // Once that's done, we have two options depending on how consistent we
      // want to be:
      // - Should the source wind up in the same state as the other cases?  If
      // so, then destroy moved from elements, free the memory, and reset the
      // invariants.
      // - Or should we leave the source as is, with moved from elements left in
      // allocated memory?  If that's the case, we're done.  I'm opting for this
      // option because I believe there's two cases here:
      // 1. The moved from container is about to die anyway, so let the
      // destructor destroy the elements and free the memory.
      // 2. If the container is going to be reused, that previously allocated
      // memory can also be reused, saving an allocation.  The user just has to
      // be aware of the fact.  Extra documentation for the move assignment
      // operator, but I believe this to be the correct choice.

      auto *new_data{std::allocator_traits<Allocator>::allocate(
          allocator_, other.capacity_)};
      try {
        allocator_aware::uninitialized_move_range(
            allocator_, other.begin(), other.end(), VectorIterator{new_data});
        allocator_aware::destroy_range(allocator_, begin(), end());
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
    allocator_aware::destroy_range(allocator_, begin(), end());
    if (data_ != nullptr) {
      std::allocator_traits<Allocator>::deallocate(allocator_, data_,
                                                   capacity_);
    }
  }

  // ** ITERATORS **
  template <typename Self>
  [[nodiscard]] constexpr auto begin(this Self &&self) noexcept {
    using const_correct_iterator =
        std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>,
                           const_iterator, iterator>;
    return static_cast<const_correct_iterator>(self.data_);
  }

  template <typename Self>
  [[nodiscard]] constexpr auto end(this Self &&self) noexcept {
    using const_correct_iterator =
        std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>,
                           const_iterator, iterator>;
    return static_cast<const_correct_iterator>(self.data_ + self.size_);
  }

  [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator {
    return begin();
  }

  [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator {
    return end();
  }

  template <typename Self>
  [[nodiscard]] constexpr auto rbegin(this Self &&self) noexcept {
    using const_correct_reverse_iterator =
        std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>,
                           const_reverse_iterator, reverse_iterator>;
    return static_cast<const_correct_reverse_iterator>(self.end());
  }

  template <typename Self>
  [[nodiscard]] constexpr auto rend(this Self &&self) noexcept {
    using const_correct_reverse_iterator =
        std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>,
                           const_reverse_iterator, reverse_iterator>;
    return static_cast<const_correct_reverse_iterator>(self.begin());
  }

  [[nodiscard]] constexpr auto crbegin() const noexcept
      -> const_reverse_iterator {
    return rbegin();
  }

  [[nodiscard]] constexpr auto crend() const noexcept
      -> const_reverse_iterator {
    return rend();
  }

  // ** ELEMENT ACCESS **
  template <typename Self>
  [[nodiscard]] constexpr auto at(this Self &&self, size_type position)
      -> decltype(auto) {
    if (position >= self.size_) {
      std::string reason{"Vector Range Check: position (which is "};
      reason += format::integral_to_string(position);
      reason += ") >= this->size() (which is ";
      reason += format::integral_to_string(self.size_);
      reason += ")";
      throw std::out_of_range(reason);
    }
    return std::forward_like<Self>(self.data_[position]);
  }

  // TODO: Add contract precondition.
  template <typename Self>
  [[nodiscard]] constexpr auto operator[](this Self &&self,
                                          size_type position) noexcept
      -> decltype(auto) {
    return std::forward_like<Self>(self.data_[position]);
  }

  // TODO: Add contract precondition.
  template <typename Self>
  [[nodiscard]] constexpr auto front(this Self &&self) noexcept
      -> decltype(auto) {
    return std::forward_like<Self>(self.data_[0]);
  }

  // TODO: Add contract precondition.
  template <typename Self>
  [[nodiscard]] constexpr auto back(this Self &&self) noexcept
      -> decltype(auto) {
    return std::forward_like<Self>(self.data_[self.size_ - 1]);
  }

  template <typename Self>
  [[nodiscard]] constexpr auto data(this Self &&self) noexcept {
    using const_correct_pointer =
        std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>,
                           const_pointer, pointer>;
    return static_cast<const_correct_pointer>(self.data_);
  }

  // ** CAPACITY **
  [[nodiscard]] constexpr auto is_empty() const noexcept -> bool {
    return size_ == 0UZ;
  }

  [[nodiscard]] constexpr auto size() const noexcept -> size_type {
    return size_;
  }

  [[nodiscard]] constexpr auto max_size() const noexcept -> size_type {
    return std::numeric_limits<size_type>::max() / sizeof(value_type);
  }

  constexpr auto reserve(size_type new_capacity) -> void {
    if (new_capacity <= capacity_) {
      return;
    }

    if (new_capacity > max_size()) {
      std::string reason{"Vector::reserve: new_capacity (which is "};
      reason += format::integral_to_string(new_capacity);
      reason += ") is greater than max_size() (which is ";
      reason += format::integral_to_string(max_size());
      reason += ").";
      throw std::length_error(reason);
    }

    auto *new_buffer{
        std::allocator_traits<Allocator>::allocate(allocator_, new_capacity)};

    migrate_data_to_new_memory(new_buffer, new_capacity);
  }

  [[nodiscard]] constexpr auto capacity() const noexcept -> size_type {
    return capacity_;
  }

  // shrink_to_fit()

  // ** MODIFIERS **
  // TODO: clear()
  // TODO: insert()
  // TODO: insert_range()
  // TODO: emplace()
  // TODO: erase()
  constexpr auto push_back(T const &value) -> void { emplace_back(value); }
  constexpr auto push_back(T const &&value) -> void {
    emplace_back(std::move(value));
  }

  template <typename... Args>
  constexpr auto emplace_back(Args &&...args) -> reference {
    if (size_ == capacity_) {
      size_type new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;

      auto new_data{
          std::allocator_traits<Allocator>::allocate(allocator_, new_capacity)};

      migrate_data_to_new_memory(new_data, new_capacity);
    }

    std::allocator_traits<Allocator>::construct(allocator_, data_ + size_,
                                                std::forward<Args>(args)...);
    return data_[size_++];
  }

  // TODO: append_range()
  // TODO: pop_back()
  // TODO: resize()

  constexpr auto swap(Vector &other) noexcept -> void {
    if constexpr (std::allocator_traits<
                      Allocator>::propagate_on_container_swap::value) {
      std::swap(allocator_, other.allocator_);
    }
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
  }

  constexpr friend auto swap(Vector &lhs, Vector &rhs) noexcept -> void {
    lhs.swap(rhs);
  }

  // ** NON-MEMBER FUNCTIONS **
  constexpr friend auto operator==(Vector const &lhs,
                                   Vector const &rhs) noexcept -> bool
    requires std::equality_comparable<T>
  {
    if (rhs.size_ != lhs.size_) {
      return false;
    }
    return std::ranges::equal(lhs, rhs);
  }

  constexpr friend auto operator<=>(Vector const &lhs,
                                    Vector const &rhs) noexcept
    requires std::three_way_comparable<T>
  {
    for (auto const pair : std::views::zip(lhs, rhs)) {
      if (auto const comparison{std::get<0>(pair) <=> std::get<1>(pair)};
          comparison != 0) {
        return comparison;
      }
    }
    return lhs.size_ <=> rhs.size_;
  }

private:
  constexpr auto migrate_data_to_new_memory(pointer destination,
                                            size_type new_capacity) -> void {
    if constexpr (std::is_nothrow_move_constructible_v<T>) {
      allocator_aware::uninitialized_move_range(
          allocator_, VectorIterator{data_}, VectorIterator{data_ + size_},
          VectorIterator{destination});
    } else if constexpr (std::is_nothrow_copy_constructible_v<T>) {
      allocator_aware::uninitialized_copy_range(
          allocator_, VectorIterator{data_}, VectorIterator{data_ + size_},
          VectorIterator{destination});
    } else {
      try {
        allocator_aware::uninitialized_copy_range(
            allocator_, VectorIterator{data_}, VectorIterator{data_ + size_},
            VectorIterator{destination});
      } catch (...) {
        // The ununitialized_copy function takes care of destroying the new
        // objects that it created during attempted migration, just have to
        // free the memory before rethrowing.
        std::allocator_traits<Allocator>::deallocate(allocator_, destination,
                                                     new_capacity);
        throw;
      }
    }

    // The uninitialized_ functions don't destroy objects unless the copy
    // fails, and even then it's only the objects that were created prior to
    // the error - not the originals.  So on successful reallocation:
    // destroy the old objects, give the memory back to the allocator, and
    // update our internals.
    allocator_aware::destroy_range(allocator_, begin(), end());
    if (data_ != nullptr) {
      std::allocator_traits<Allocator>::deallocate(allocator_, data_,
                                                   capacity_);
    }
    data_ = destination;
    capacity_ = new_capacity;
  }

  [[no_unique_address]] Allocator allocator_;
  T *data_{};
  std::size_t capacity_{};
  std::size_t size_{};
};

// Explicit Deduction Guide for CTAD.
template <std::input_iterator InputIterator,
          std::sentinel_for<InputIterator> Sentinel>
Vector(InputIterator, Sentinel) -> Vector<std::iter_value_t<InputIterator>>;

} // namespace swtl
