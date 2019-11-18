#pragma once
#include "_o_.hpp"
#include <type_traits>
template <typename...> struct purry;
template <typename... Args> purry(Args...)->purry<Args...>;
template <typename Pith> struct purry<Pith> {
  Pith pith;
  template <typename... Args> constexpr void operator()(Args &&... args) const {
    pith(static_cast<Args &&>(args)...);
  }
};
template <typename Pith, typename A> struct purry<Pith, A> {
  Pith pith;
  A a;
  template <typename O, typename... Rest>
  constexpr void operator()(O &&o, Rest &&... rest) const {
    if constexpr (std::is_invocable_r_v<void, A, void (*)(...)>)
      a(_o_{[&](int err) { o(err); },
            [&, ... rest = static_cast<Rest &&>(rest)](auto &&... a) {
              pith(o, static_cast<decltype(a) &&>(a)..., rest...);
            }});
    else
      pith(static_cast<O &&>(o), a, static_cast<Rest &&>(rest)...);
  }
};
template <typename R, typename... Args>
constexpr auto operator^(purry<Args...> l, R &&r) {
  return purry{l, static_cast<R &&>(r)};
}
template <typename R, typename... Args>
constexpr auto operator|(purry<Args...> l, R &&r) {
  return purry{static_cast<R &&>(r), l};
}
