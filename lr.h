#ifndef LR_H
#define LR_H 1
#include "overloaded.h"
#include <string>
#include <variant>

namespace lr {

struct L {
  template <typename T,
            typename = std::enable_if_t<std::is_convertible_v<T, std::string>>>
  L(T &&a) : message(std::forward<T>(a)) {}

private:
  std::string message;
};

template <typename... Rights> using LR = std::variant<L, Rights...>;

template <typename OSet, typename LRType>
constexpr decltype(auto) map(OSet &&os, LRType &&lr) {
  using R = decltype(std::declval<OSet>()(std::get<1>(std::declval<LRType>())));
  return std::visit(overloaded{[](L &&l) { return LR<R>(std::forward<L>(l)); },
                               [&os](auto &&a) {
                                 return LR<R>(os(std::forward<decltype(a)>(a)));
                               }},
                    std::forward<LRType>(lr));
}

template <typename OSet, typename LRType>
constexpr decltype(auto) flatMap(OSet &&os, LRType &&lr) {
  using R = decltype(std::declval<OSet>()(std::get<1>(std::declval<LRType>())));
  return std::visit(
      overloaded{[](L &&l) { return R(std::forward<L>(l)); },
                 [&os](auto &&a) { return os(std::forward<decltype(a)>(a)); }},
      std::forward<LRType>(lr));
}

} // namespace lr
#endif