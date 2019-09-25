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

template <typename OSet> struct fmap {
  using O = OSet;
  O o;
  template <typename U> fmap(U &&rhs) : o(std::forward<U>(rhs)) {}

  template <typename... As,
            typename A1 = decltype(std::get<1>(std::declval<lr::LR<As...>>())),
            typename R = decltype(std::declval<OSet>()(std::declval<A1>()))>
  constexpr decltype(auto) operator()(LR<As...> &&lr) const noexcept {
    return std::visit(
        [&](auto &&a) {
          using Any = decltype(a);
          using T = std::decay_t<Any>;
          if constexpr (std::is_same_v<T, L>)
            return LR<R>(std::forward<Any>(a));
          else
            return LR<R>(o(std::forward<Any>(a)));
        },
        std::forward<LR<As...>>(lr));
  }
};

template <class... Ts> fmap(Ts...)->fmap<Ts...>;

static void test() { //
  auto f = fmap(overloaded{[](int a) { return 1; }, [](float a) { return 2; }})(
      lr::LR<int, float>{1.f});
  std::string a;
};

template <typename LRType, typename OSet>
constexpr decltype(auto) map(OSet &&os, LRType &&lr) {
  using R = decltype(std::declval<OSet>()(std::get<1>(std::declval<LRType>())));
  return std::visit(overloaded{[](L &&l) { return LR<R>(std::forward<L>(l)); },
                               [&os](auto &&a) {
                                 return LR<R>(os(std::forward<decltype(a)>(a)));
                               }},
                    std::forward<LRType>(lr));
}

template <typename OSet, typename LRType0, typename LRType1>
constexpr decltype(auto) map2(OSet &&os, LRType0 &&lr0, LRType1 &&lr1) {
  using R =
      decltype(std::declval<OSet>()(std::get<1>(std::declval<LRType0>()),
                                    std::get<1>(std::declval<LRType1>())));
  return std::visit(
      overloaded{[](L &&l, auto) { return LR<R>(std::forward<L>(l)); },
                 [](auto, L &&l) { return LR<R>(std::forward<L>(l)); },
                 [](L &&l0, L &&l1) { return LR<R>(l0 + l1); },
                 [&os](auto &&a, auto &&b) {
                   return LR<R>(os(std::forward<decltype(a)>(a),
                                   std::forward<decltype(b)>(b)));
                 }},
      std::forward<LRType0>(lr0), std::forward<LRType1>(lr1));
}

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

template <typename OSet, typename LRType0, typename LRType1>
constexpr decltype(auto) flatMap2(OSet &&os, LRType0 &&lr0, LRType1 &&lr1) {
  using R =
      decltype(std::declval<OSet>()(std::get<1>(std::declval<LRType0>()),
                                    std::get<1>(std::declval<LRType1>())));
  return std::visit(
      overloaded{[](L &&l, auto) { return R(std::forward<L>(l)); },
                 [](auto, L &&l) { return R(std::forward<L>(l)); },
                 [](L &&l0, L &&l1) { return R(l0 + l1); },
                 [&os](auto &&a, auto &&b) {
                   return os(std::forward<decltype(a)>(a),
                             std::forward<decltype(b)>(b));
                 }},
      std::forward<LRType0>(lr0), std::forward<LRType1>(lr1));
}
} // namespace lr
#endif
