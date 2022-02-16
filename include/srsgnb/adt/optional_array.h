
#ifndef SRSGNB_OPTIONAL_ARRAY_H
#define SRSGNB_OPTIONAL_ARRAY_H

#include "optional.h"
#include "span.h"
#include "srsgnb/support/srsran_assert.h"
#include <array>

namespace srsgnb {

namespace detail {

/// Base class used for the implementation of the common functionality in optional_array<>, optional_vector<> and
/// optional_span<>.
/// \tparam Vec type of container used
template <typename Vec>
class base_optional_span
{
  using base_t = base_optional_span<Vec>;
  using T      = typename Vec::value_type::value_type;

protected:
  /// Iterator implementation
  /// \tparam Obj either const T or T
  template <typename Obj>
  class iterator_impl
  {
    using It       = iterator_impl<Obj>;
    using parent_t = typename std::conditional<std::is_const<Obj>::value, const base_t, base_t>::type;

  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = Obj;
    using difference_type   = std::ptrdiff_t;
    using pointer           = Obj*;
    using reference         = Obj&;

    iterator_impl() = default;
    iterator_impl(parent_t* parent_, size_t idx_) : parent(parent_), idx(idx_)
    {
      if (idx < parent->vec.size() and not parent->contains(idx)) {
        ++(*this);
      }
    }

    It& operator++()
    {
      while (++idx < parent->vec.size() and not parent->contains(idx)) {
      }
      return *this;
    }
    It& operator--()
    {
      while (--idx < parent->vec.size() and not parent->contains(idx)) {
      }
      return *this;
    }

    reference operator*() { return parent->operator[](idx); }
    pointer   operator->() { return &parent->operator[](idx); }

    bool operator==(const It& other) const { return idx == other.idx and parent == other.parent; }
    bool operator!=(const It& other) const { return not(*this == other); }

  private:
    friend base_t;

    parent_t* parent = nullptr;
    size_t    idx    = std::numeric_limits<size_t>::max();
  };

  template <typename It>
  static size_t extract_iterator_index(It it)
  {
    return it.idx;
  }

  size_t nof_elems = 0; ///< Number of present fields
  Vec    vec;           ///< Container to store optional elements

public:
  using value_type     = T;
  using iterator       = iterator_impl<T>;
  using const_iterator = iterator_impl<const T>;

  constexpr base_optional_span() noexcept = default;
  base_optional_span(Vec&& v, size_t nof_elems_) : vec(std::move(v)), nof_elems(nof_elems_) {}
  base_optional_span(const Vec& v, size_t nof_elems_) : vec(v), nof_elems(nof_elems_) {}

  bool contains(size_t idx) const noexcept { return idx < vec.size() and vec[idx].has_value(); }

  T&       operator[](size_t idx) noexcept { return *vec[idx]; }
  const T& operator[](size_t idx) const noexcept { return *vec[idx]; }

  constexpr bool   empty() const noexcept { return nof_elems == 0; }
  constexpr size_t size() const noexcept { return nof_elems; }

  iterator       begin() { return iterator{this, 0}; }
  iterator       end() { return iterator{this, vec.size()}; }
  const_iterator begin() const { return const_iterator{this, 0}; }
  const_iterator end() const { return const_iterator{this, vec.size()}; }

  /// Find first position that is empty
  size_t find_first_empty(size_t start_guess = 0)
  {
    if (nof_elems == vec.size()) {
      return vec.size();
    }
    for (size_t i = start_guess; i < vec.size(); ++i) {
      if (not vec[i].has_value()) {
        return i;
      }
    }
    return vec.size();
  }
};

/// Base class used for the implementation of the common functionality in optional_array<>, optional_vector<>.
template <typename Vec>
class base_optional_vector : public base_optional_span<Vec>
{
  using base_t = base_optional_span<Vec>;

public:
  using value_type     = typename base_optional_span<Vec>::value_type;
  using iterator       = typename base_optional_span<Vec>::iterator;
  using const_iterator = typename base_optional_span<Vec>::const_iterator;

  constexpr base_optional_vector()                  = default;
  base_optional_vector(const base_optional_vector&) = default;
  base_optional_vector(base_optional_vector&& other) noexcept : base_t(std::move(other.vec), other.size())
  {
    other.nof_elems = 0;
  }
  base_optional_vector& operator=(const base_optional_vector&) = default;
  base_optional_vector& operator                               =(base_optional_vector&& other) noexcept
  {
    this->vec       = std::move(other.vec);
    this->nof_elems = other.nof_elems;
    other.nof_elems = 0;
    return *this;
  }

  /// Clear all elements of the container
  void clear() noexcept
  {
    this->nof_elems = 0;
    for (auto& e : *this) {
      e.reset();
    }
  }

  /// Erase object pointed by the given index
  /// \param idx Position of the erased element in the array
  void erase(size_t idx) noexcept
  {
    srsran_assert(idx < this->vec.size(), "Out-of-bounds access to array: %zd>=%zd", idx, this->vec.size());
    if (this->contains(idx)) {
      this->nof_elems--;
      this->vec[idx].reset();
    }
  }

  /// Erase object pointed by the given iterator. Iterator must point to valid element
  /// \param it container iterator
  void erase(iterator it) noexcept { erase(this->extract_iterator_index(it)); }

  /// Insert given object in given index
  /// \param idx Position of the constructed element in the array
  /// \param u object to insert
  template <typename U>
  void insert(size_t idx, U&& u)
  {
    static_assert(std::is_convertible<U, value_type>::value, "Ctor T(U&&) does not exist.");
    srsran_assert(idx < this->vec.size(), "Out-of-bounds access to array: %zd>=%zd", idx, this->vec.size());
    this->nof_elems += this->contains(idx) ? 0 : 1;
    this->vec[idx] = std::forward<U>(u);
  }

  /// Overwrite array element with given index with a newly constructed object
  /// \param idx Position of the constructed element in the array
  /// \param args Arguments to pass to element ctor
  template <typename... Args>
  void emplace(size_t idx, Args&&... args)
  {
    static_assert(std::is_constructible<value_type, Args&&...>::value, "Ctor T(Args...) does not exist.");
    srsran_assert(idx < this->vec.size(), "Out-of-bounds access to array: %zd>=%zd", idx, this->vec.size());
    this->nof_elems += this->contains(idx) ? 0 : 1;
    this->vec[idx].emplace(std::forward<Args>(args)...);
  }
};

} // namespace detail

/// Array of optional items. The iteration is in order of indexes and correctly skips absent elements.
/// Pointer/References/Iterators remain valid throughout the object lifetime.
/// NOTE: The sorted iteration and pointer validation guarantees may add some overhead if the array is very fragmented.
/// @tparam T type of objects
/// @tparam N static size of max nof items
template <typename T, size_t N>
class optional_array : private detail::base_optional_vector<std::array<optional<T>, N> >
{
  using base_t = detail::base_optional_vector<std::array<optional<T>, N> >;

public:
  using value_type     = typename base_t::value_type;
  using iterator       = typename base_t::iterator;
  using const_iterator = typename base_t::const_iterator;

  using base_t::base_t;
  using base_t::operator=;
  using base_t::contains;
  using base_t::operator[];
  using base_t::begin;
  using base_t::clear;
  using base_t::emplace;
  using base_t::empty;
  using base_t::end;
  using base_t::erase;
  using base_t::find_first_empty;
  using base_t::insert;
  using base_t::size;
};

/// Contrarily to optional_array, this class may allocate and cause pointer/reference/iterator invalidation.
/// However, the indexes will remain valid.
/// @tparam T
template <typename T>
class optional_vector : private detail::base_optional_vector<std::vector<optional<T> > >
{
  using base_t = detail::base_optional_vector<std::vector<optional<T> > >;

public:
  using value_type     = typename base_t::value_type;
  using iterator       = typename base_t::iterator;
  using const_iterator = typename base_t::const_iterator;

  using base_t::base_t;
  using base_t::operator=;
  using base_t::contains;
  using base_t::operator[];
  using base_t::begin;
  using base_t::clear;
  using base_t::empty;
  using base_t::end;
  using base_t::erase;
  using base_t::find_first_empty;
  using base_t::size;

  /// May allocate and cause pointer invalidation
  template <typename U>
  void insert(size_t idx, U&& u)
  {
    if (idx >= this->vec.size()) {
      this->vec.resize(idx + 1);
    }
    base_t::insert(idx, std::forward<U>(u));
  }

  /// May allocate and cause pointer invalidation
  template <typename... Args>
  void emplace(size_t idx, Args&&... args)
  {
    if (idx >= this->vec.size()) {
      this->vec.resize(idx + 1);
    }
    base_t::emplace(idx, std::forward<Args>(args)...);
  }
};

/// View handle to inspect individual present elements of an optional array. It doesn't provide an insertion/deletion
/// interface.
/// \tparam T type of elements
template <typename T>
class optional_span : public detail::base_optional_span<srsgnb::span<optional<T> > >
{
  using base_t = detail::base_optional_span<srsgnb::span<optional<T> > >;

public:
  template <size_t N>
  optional_span(const optional_array<T, N>& ar) : base_t::vec(ar)
  {}
  optional_span(const optional_vector<T>& ar) : base_t::vec(ar) {}
};

namespace detail {

template <typename T>
class base_split_optional_span
{
protected:
  using presence_type = typename std::conditional<std::is_const<T>::value, const bool, bool>::type;

  T*             ptr         = nullptr;
  presence_type* present_ptr = nullptr;
  size_t         len         = 0;

  template <typename Obj>
  class iterator_impl
  {
    using It     = iterator_impl<Obj>;
    using Parent = typename std::
        conditional<std::is_const<Obj>::value, const base_split_optional_span<T>, base_split_optional_span<T> >::type;

  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = Obj;
    using difference_type   = std::ptrdiff_t;
    using pointer           = Obj*;
    using reference         = Obj&;

    iterator_impl() = default;
    iterator_impl(Parent* parent_, size_t idx_) : parent(parent_), idx(idx_)
    {
      if (idx < parent->len and not parent->contains(idx)) {
        ++(*this);
      }
    }

    It& operator++()
    {
      while (++idx < parent->len and not parent->contains(idx)) {
      }
      return *this;
    }
    It& operator--()
    {
      while (--idx < parent->len and not parent->contains(idx)) {
      }
      return *this;
    }

    reference operator*() { return parent->operator[](idx); }
    pointer   operator->() { return &parent->operator[](idx); }

    bool operator==(const It& other) const { return idx == other.idx and parent == other.parent; }
    bool operator!=(const It& other) const { return not(*this == other); }

    size_t get_idx() const { return idx; }

  private:
    Parent* parent = nullptr;
    size_t  idx    = std::numeric_limits<size_t>::max();
  };

public:
  using value_type     = T;
  using iterator       = iterator_impl<T>;
  using const_iterator = iterator_impl<const T>;

  constexpr base_split_optional_span() = default;
  template <std::size_t N>
  constexpr base_split_optional_span(value_type (&arr)[N], presence_type (&present)[N]) noexcept : ptr(arr),
                                                                                                   present_ptr(present),
                                                                                                   len(N)
  {}
  constexpr base_split_optional_span(value_type* arr, presence_type* present, size_t N) :
    ptr(arr), present_ptr(present), len(N)
  {}

  bool contains(size_t idx) const { return idx < len and present_ptr[idx]; }
  bool empty() const
  {
    for (size_t i = 0; i < len; ++i) {
      if (present_ptr[i]) {
        return false;
      }
    }
    return true;
  }
  size_t size() const
  {
    size_t c = 0;
    for (size_t i = 0; i < len; ++i) {
      c += present_ptr[i] ? 1 : 0;
    }
    return c;
  }
  size_t capacity() const { return len; }

  const T& operator[](size_t idx) const { return ptr[idx]; }
  T&       operator[](size_t idx) { return ptr[idx]; }
  const T& at(size_t idx) const
  {
    srsran_assert(contains(idx), "Access to inexistent element of index=%zd", idx);
    return ptr[idx];
  }
  T& at(size_t idx)
  {
    srsran_assert(this->contains(idx), "Access to inexistent element of index=%zd", idx);
    return this->ptr[idx];
  }

  const_iterator begin() const { return const_iterator(this, 0); }
  const_iterator end() const { return const_iterator(this, len); }
  iterator       begin() { return iterator(this, 0); }
  iterator       end() { return iterator(this, this->len); }

  // Find first position that is empty
  size_t find_first_empty(size_t start_guess = 0) { return begin().get_idx(); }
};

} // namespace detail

template <typename T>
class split_optional_span : public detail::base_split_optional_span<T>
{
  using base_t = detail::base_split_optional_span<T>;

public:
  using value_type     = T;
  using const_iterator = typename base_t::const_iterator;
  using iterator       = typename base_t::iterator;

  using base_t::base_t;

  template <typename U>
  void insert(size_t idx, U&& u)
  {
    srsran_assert(idx < this->len, "Out-of-bounds access to array: %zd>=%zd", idx, this->len);
    this->present_ptr[idx] = true;
    this->ptr[idx]         = std::forward<U>(u);
  }
  void erase(size_t idx)
  {
    srsran_assert(idx < this->len, "Out-of-bounds access to array: %zd>=%zd", idx, this->len);
    this->present_ptr[idx] = false;
  }
  void erase(iterator it) { erase(it.get_idx()); }
  void clear()
  {
    for (size_t i = 0; i < this->len; ++i) {
      this->present_ptr[i] = false;
    }
  }
};

template <typename U>
class split_optional_span<const U> : public detail::base_split_optional_span<const U>
{
  using base_t        = detail::base_split_optional_span<const U>;
  using presence_type = typename base_t::presence_type;

public:
  using value_type     = const U;
  using const_iterator = typename base_t::const_iterator;

  using base_t::base_t;
};

template <typename T>
split_optional_span<T>
make_optional_span(T*                                                                          array,
                   typename std::conditional<std::is_const<T>::value, const bool, bool>::type* present,
                   size_t                                                                      N)
{
  return split_optional_span<T>(array, present, N);
}
template <typename T, size_t N>
split_optional_span<T>
make_optional_span(T (&array)[N],
                   typename std::conditional<std::is_const<T>::value, const bool, bool>::type (&present)[N])
{
  return split_optional_span<T>(array, present);
}

} // namespace srsgnb

#endif // SRSGNB_OPTIONAL_ARRAY_H
