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

template <typename F> struct fmap {
  F f;
  fmap(F &&r) : f(std::forward<F>(r)) {}

  template <typename T, typename = std::enable_if_t<islr_v<T>>>
  constexpr decltype(auto) operator()(T &&lr) const {
    using R = decltype(std::declval<F>()(std::get<1>(std::declval<T>())));
    return std::visit(
        overloaded{
            [](L &&l) { return LR<R>(std::forward<L>(l)); },
            [&](auto &&a) { return LR<R>(f(std::forward<decltype(a)>(a))); }},
        std::forward<T>(lr));
  }
};
template <typename F> fmap(F)->fmap<F>;

// template <typename... LRType> constexpr auto all(LRType &&... lr) {}

template <typename OSet, typename LRType>
constexpr decltype(auto) flatMap(OSet &&os, LRType &&lr) {
  return std::visit(
      overloaded{[](L &&l) {
                   return decltype(std::declval<OSet>()(std::get<1>(
                       std::declval<LRType>())))(std::forward<L>(l));
                 },
                 [&os](auto &&a) { return os(std::forward<decltype(a)>(a)); }},
      std::forward<LRType>(lr));
}

} // namespace lr
#endif
