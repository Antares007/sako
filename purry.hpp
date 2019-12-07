#pragma once
#include "_o_.hpp"

template <typename Pith, typename L, typename A> struct purry {
  Pith pith;
  A a;
  template <typename O, typename... Rest>
  void operator()(O o, Rest &&... rest) const {
    if constexpr (std::is_invocable_r_v<void, A, void (*)(...)>)
      a(_o_{[&o](L err) { o(err); },
            [&o, this, ... rest = static_cast<Rest &&>(rest)](auto &&... a) {
              this->pith(o, static_cast<decltype(a) &&>(a)..., rest...);
            }});
    else
      pith(static_cast<O &&>(o), a, static_cast<Rest &&>(rest)...);
  }
};

template <typename Pith, typename L> struct purry<Pith, L, void> : Pith {
  using Pith::operator();
};

template <typename Pith> purry(Pith)->purry<Pith, int, void>;

template <typename Pith, typename L, typename A>
constexpr auto operator^(purry<Pith, L, void> l, A &&a) {
  return purry<Pith, L, A>{l, static_cast<A &&>(a)};
}

template <typename Pith, typename L, typename A, typename R>
constexpr auto operator^(purry<Pith, L, A> l, R &&r) {
  return purry<purry<Pith, L, A>, L, R>{l, static_cast<R &&>(r)};
}

template <typename Pith, typename L, typename R>
constexpr auto operator|(purry<Pith, L, void> l, R &&r) {
  return purry<R, L, Pith>{static_cast<R &&>(r), l};
}

template <typename Pith, typename L, typename A, typename R>
constexpr auto operator|(purry<Pith, L, A> l, R &&r) {
  return purry<R, L, purry<Pith, L, A>>{static_cast<R &&>(r), l};
}
