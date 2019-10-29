#pragma once

#include "_o_.hpp"
#include <functional>

template <typename Pith, typename A> struct pin {
  Pith pith;
  A a;
  template <typename O, typename... Rest>
  constexpr void operator()(const O &o, Rest &&... rest) const {
    auto r = _o_{[&](int err) { o(err); },
                 [&, ... rest = std::forward<Rest>(rest)](auto &&... a) {
                   pith(o, std::forward<decltype(a)>(a)..., rest...);
                 }};
    if constexpr (std::is_invocable_r_v<void, A, decltype(r)>)
      a(r);
    else
      pith(o, a, rest...);
  }
};
template <typename Pith, typename A> pin(Pith, A)->pin<Pith, A>;

template <typename Pith, typename A, typename R>
constexpr auto operator^(pin<Pith, A> l, R &&r) {
  return pin{l, std::forward<R>(r)};
}
