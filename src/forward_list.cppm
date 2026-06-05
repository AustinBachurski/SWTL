export module swtl_forward_list;

import std;

export template <typename T> class Forward_List {
public:
  Forward_List() = default;

  Forward_List(Forward_List const &other) {
    auto pos{before_begin()};

    for (auto const &element : other) {
      pos = insert_after(pos, element);
    }
  }

  Forward_List &operator=(Forward_List const &other) {
    if (*this == other) {
      return *this;
    }

    clear();

    auto pos{before_begin()};

    for (auto const &element : other) {
      pos = insert_after(pos, element);
    }

    return *this;
  }

  Forward_List(Forward_List &&other) noexcept {
    head_.next_ = other.head_.next_;
    size_ = other.size_;
    other.head_.next_ = nullptr;
    other.size_ = 0uz;
  }

  Forward_List &operator=(Forward_List &&other) noexcept {
    if (*this == other) {
      return *this;
    }

    clear();
    head_.next_ = other.head_.next_;
    size_ = other.size_;
    other.head_.next_ = nullptr;
    other.size_ = 0uz;
    return *this;
  }

  ~Forward_List() { clear(); }

private:
  struct NodeBase {
    NodeBase *next_{};
  };

  struct Node : public NodeBase {
    Node(T const &value) : data_{value} {}

    T data_{};
  };

public:
  class Iterator {
    friend class Forward_List;

    using iterator_category = std::forward_iterator_tag;
    using value_type = std::remove_cv_t<T>;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;

  public:
    Iterator &operator++() {
      ptr_ = ptr_->next_;
      return *this;
    }

    Iterator operator++(int) {
      auto tmp{*this};
      ptr_ = ptr_->next_;
      return tmp;
    }

    bool operator==(Iterator const &other) const { return ptr_ == other.ptr_; }

    bool operator!=(Iterator const &other) const { return ptr_ != other.ptr_; }

    reference operator*() { return static_cast<Node *>(ptr_)->data_; }

    value_type operator*() const {
      return static_cast<Node const *>(ptr_)->data_;
    }

    pointer *operator->() { return &static_cast<Node *>(ptr_)->data_; }

  private:
    Iterator(NodeBase *value) : ptr_{value} {}

    NodeBase *ptr_{};
  };

public:
  void clear() {
    while (head_.next_) {
      auto *tmp{head_.next_};
      head_.next_ = head_.next_->next_;

      delete static_cast<Node *>(tmp);
    }

    head_.next_ = nullptr;
    size_ = 0uz;
  }

  Iterator insert_after(Iterator const pos, T const &value) {
    auto elem{new Node(value)};
    elem->next_ = pos.ptr_->next_;
    pos.ptr_->next_ = elem;
    ++size_;

    return {elem};
  }

  [[nodiscard]]
  bool is_empty() {
    return size_ == 0uz;
  }

  void push_front(T const &value) { insert_after(before_begin(), value); }

  [[nodiscard]]
  std::size_t size() const {
    return size_;
  }

public:
  auto operator==(Forward_List const &other) const {
    return head_.next_ == other.head_.next_;
  }

public:
  Iterator before_begin() { return Iterator(&head_); }
  Iterator before_begin() const { return Iterator(&head_); }
  Iterator cbefore_begin() const { return Iterator(&head_); }

public:
  Iterator begin() { return Iterator(head_.next_); }
  Iterator end() { return Iterator(nullptr); }
  Iterator begin() const { return Iterator(head_.next_); }
  Iterator end() const { return Iterator(nullptr); }
  Iterator cbegin() const { return Iterator(head_.next_); }
  Iterator cend() const { return Iterator(nullptr); }

private:
  NodeBase head_{};
  std::size_t size_{};
};
