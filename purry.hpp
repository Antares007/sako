#pragma once

#include "ob.hpp"

struct error_ray;
constexpr inline error_ray *error_ray_v = nullptr;

template <typename Pith, typename A> struct purry {
  Pith pith;
  A a;
  MOB(typename... Rest)(Rest &&... rest) {
    if constexpr (std::is_invocable_r_v<void, A, void (*)(...)>)
      a(_o_{[&o](error_ray *l, auto &&... rest) {
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
