#pragma once

#include "_o_.hpp"
#include <type_traits>

template <typename...> struct pin;
template <typename Pith, typename A> struct pin<Pith, A> {
  Pith pith;
  A a;
  template <typename O, typename... Rest>
  constexpr void operator()(const O &o, Rest &&... rest) const {
    if constexpr (std::is_invocable_r_v<void, A, void (*)(...)>)
      a(_o_{[&](int err) { o(err); },
            [&, ... rest = static_cast<Rest &&>(rest)](auto &&... a) {
              pith(o, static_cast<decltype(a) &&>(a)..., rest...);
            }});
    else
      pith(o, a, static_cast<Rest &&>(rest)...);
  }
};

template <typename Pith, typename A, typename B> struct pin<Pith, A, B> {
  Pith pith;
  A a;
  B b;
  template <typename O, typename... Rest>
  constexpr void operator()(const O &o, Rest &&... rest) const {
    pin<pin<Pith, A>, B>{pin<Pith, A>{pith, a},
                         b}(o, static_cast<Rest &&>(rest)...);
  }
};
template <typename... Args> pin(Args...)->pin<Args...>;

template <typename Pith, typename A, typename R>
constexpr auto operator^(pin<Pith, A> l, R &&r) {
  return pin{l, static_cast<R &&>(r)};
}
template <typename Pith, typename A, typename R>
constexpr auto operator|(pin<Pith, A> l, R &&r) {
  return pin{static_cast<R &&>(r), l};
}
