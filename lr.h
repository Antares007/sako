#ifndef LR_H
#define LR_H 1
#include "overloaded.h"
#include <string>
#include <variant>

namespace lr {

struct L {
  L() = delete;
  L(const L &) = delete;
  L(L &&) = default;
  template <typename T,
            typename = std::enable_if_t<std::is_convertible_v<T, std::string>>>
  L(T &&a) : message(std::forward<T>(a)) {}
  L operator+(const L &other) { return L(this->message + other.message); }

private:
  std::string message;
};

template <typename... Rights> using LR = std::variant<L, Rights...>;

template <typename T> struct islr : std::false_type {};
template <typename... R> struct islr<std::variant<L, R...>> : std::true_type {};
template <typename T> using islr_t = typename islr<T>::type;
template <typename T> inline constexpr bool islr_v = islr<T>::value;

template <typename T> inline constexpr std::variant<L, T> create(T &&t) {
  return LR<T>(std::forward<T>(t));
}

template <typename F> struct fmap {
  F f;
  fmap(F &&r) : f(std::forward<F>(r)) {}

  template <typename T, typename = std::enable_if_t<islr_v<T>>>
  constexpr decltype(auto) operator()(T &&lr) const {
    using R = decltype(std::declval<F>()(std::get<1>(std::declval<T>())));
    if constexpr (islr_v<R>)
      return std::visit(
          overloaded{[](L &&l) { return R(std::forward<L>(l)); },
                     [&](auto &&a) { return f(std::forward<decltype(a)>(a)); }},
          std::forward<T>(lr));
    else
      return std::visit(
          overloaded{
              [](L &&l) { return LR<R>(std::forward<L>(l)); },
              [&](auto &&a) { return LR<R>(f(std::forward<decltype(a)>(a))); }},
          std::forward<T>(lr));
  }
};

} // namespace lr
#endif
