#pragma once

template <class... Ts> struct rays : Ts... { using Ts::operator()...; };
template <class... Ts> rays(Ts...) -> rays<Ts...>;

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

struct next_ray;
template <typename L, typename R> struct cont {
  L l;
  R r;
  template <typename O, typename... Args>
  void operator()(const O &o, Args &&... args) const { //
    bool error = false;
    l(::rays{[&](error_ray *tag, int err) {
               error = true;
               o(tag, err);
             },
             [&]<typename V>(V &&v) { o(static_cast<V &&>(v)); }},
      static_cast<Args &&>(args)...);
    if (!error)
      o(static_cast<next_ray *>(nullptr), r);
  }
};
template <typename... Args> cont(Args...) -> cont<Args...>;

template <typename L, typename R> constexpr auto operator>>=(L &&l, R &&r) {
  return cont<L, R>{static_cast<L &&>(l), static_cast<R &&>(r)};
}

template <typename Pith, typename A>
constexpr auto operator,(Pith &&pith, A &&a) {
  return curry<Pith, A>{static_cast<Pith &&>(pith), static_cast<A &&>(a)};
}

template <typename Pith, typename A>
constexpr auto operator^(Pith &&pith, A &&a) {
  return purry<Pith, A>{static_cast<Pith &&>(pith), static_cast<A &&>(a)};
}
