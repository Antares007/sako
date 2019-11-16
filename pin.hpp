#pragma once

#include "_o_.hpp"
#include <type_traits>

template <typename Pith, typename A> struct pin {
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
template <typename Pith, typename A> pin(Pith, A)->pin<Pith, A>;

template <typename Pith, typename A, typename R>
constexpr auto operator^(pin<Pith, A> l, R &&r) {
  return pin{l, static_cast<R &&>(r)};
}
template <typename Pith, typename A, typename R>
constexpr auto operator|(pin<Pith, A> l, R &&r) {
  return pin{static_cast<R &&>(r), l};
}
