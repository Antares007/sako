#ifndef ABO_NEWTYPE_HPP
#define ABO_NEWTYPE_HPP

#include <type_traits>
#include <utility>

#define NewType(name, ...)                                                     \
  struct name : abo::nt::newtype<struct name##Tag, __VA_ARGS__> {              \
    using abo::nt::newtype<struct name##Tag, __VA_ARGS__>::newtype;            \
  }

namespace abo::nt {

template <typename Tag, typename T, template <typename> typename... Skills>
struct newtype : Skills<newtype<Tag, T, Skills...>>... {
  template <typename U,
            typename = std::enable_if_t<std::is_convertible_v<U, T>>>
  constexpr newtype(U &&u) : value(std::forward<U>(u)) {}
  constexpr T const &get() const { return value; }

private:
  T value;
};

template <typename T, template <typename> class crtpType> struct crtp {
  T &underlying() { return static_cast<T &>(*this); }
  T const &underlying() const { return static_cast<T const &>(*this); }
};

template <typename T> struct Addable : crtp<T, Addable> {
  T operator+(T const &other) const {
    return T(this->underlying().get() + other.get());
  }
};

} // namespace abo::nt
#endif
