#pragma once

template <class... Ts> struct rays : Ts... { using Ts::operator()...; };
template <class... Ts> rays(Ts...)->rays<Ts...>;

struct error_ray;
constexpr inline error_ray *error_ray_v = nullptr;

template <typename Pith, typename A> struct curry {
  Pith pith;
  A a;
  template <typename O, typename... Rest>
  void operator()(O o, Rest &&... rest) const {
    pith(o, a, static_cast<Rest &&>(rest)...);
  }
};

template <typename Pith, typename A> struct purry {
  Pith pith;
  A a;
  template <typename O, typename... Rest>
  void operator()(O o, Rest &&... rest) const {
    a(rays{[&o](error_ray *l, auto &&... rest) {
             o(l, static_cast<decltype(rest) &&>(rest)...);
           },
           [&o, this, ... rest = static_cast<Rest &&>(rest)](auto &&... a) {
             this->pith(o, static_cast<decltype(a) &&>(a)..., rest...);
           }});
  }
};

template <typename Pith, typename A>
constexpr auto operator,(Pith &&pith, A &&a) {
  return curry<Pith, A>{static_cast<Pith &&>(pith), static_cast<A &&>(a)};
}

template <typename Pith, typename A>
constexpr auto operator^(Pith &&pith, A &&a) {
  return purry<Pith, A>{static_cast<Pith &&>(pith), static_cast<A &&>(a)};
}
