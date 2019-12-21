#pragma once

#include "_o_.hpp"

struct left_ray;
constexpr inline left_ray *left_ray_v = nullptr;

template <typename Pith, typename A> struct purry {
  Pith pith;
  A a;
  template <typename O, typename... Rest>
  void operator()(const O &o, Rest &&... rest) const {
    if constexpr (std::is_invocable_r_v<void, A, void (*)(...)>)
      a(_o_{[&o](left_ray *l, auto &&... rest) {
              o(l, static_cast<decltype(rest) &&>(rest)...);
            },
            [&o, this, ... rest = static_cast<Rest &&>(rest)](auto &&... a) {
              this->pith(o, static_cast<decltype(a) &&>(a)..., rest...);
            }});
    else
      pith(o, a, static_cast<Rest &&>(rest)...);
  }
};

template <typename Pith, typename A>
constexpr auto operator^(Pith &&pith, A &&a) {
  return purry<Pith, A>{static_cast<Pith &&>(pith), static_cast<A &&>(a)};
}
