#pragma once
#include "_o_.hpp"

template <typename...> struct purry;
template <typename... Args> purry(Args...)->purry<Args...>;
template <typename Pith> struct purry<Pith> : Pith { using Pith::operator(); };
template <typename Pith, typename A> struct purry<Pith, A> {
  Pith pith;
  A a;
  template <typename O, typename... Rest>
  constexpr void operator()(O &&o, Rest &&... rest) const {
    if constexpr (std::is_invocable_r_v<void, A, void (*)(...)>)
      a(_o_{[&](int err) { o(err); },
            [pith = this->pith, o = static_cast<O &&>(o),
             ... rest = static_cast<Rest &&>(rest)](auto &&... a) {
              pith(o, static_cast<decltype(a) &&>(a)..., rest...);
            }});
    else
      pith(static_cast<O &&>(o), a, static_cast<Rest &&>(rest)...);
  }
};

template <typename Pith, typename A, typename B> struct purry<Pith, A, B> {
  Pith pith;
  A a;
  B b;
  template <typename O, typename... Rest>
  constexpr void operator()(O &&o, Rest &&... rest) const {
    if constexpr (std::is_invocable_r_v<void, A, void (*)(...)> &&
                  std::is_invocable_r_v<void, B, void (*)(...)>)
      a(_o_{[&](int err) { o(err); },
            [&, ... rest = static_cast<Rest &&>(rest)](auto &&... a) {
              b(_o_{[&](int err) { o(err); },
                    [&, ... a = static_cast<decltype(a) &&>(a)](auto &&... b) {
                      pith(o, a..., static_cast<decltype(b) &&>(b)..., rest...);
                    }});
            }});
    else if constexpr (std::is_invocable_r_v<void, A, void (*)(...)>)
      a(_o_{[&](int err) { o(err); },
            [&, ... rest = static_cast<Rest &&>(rest)](auto &&... a) {
              this->pith(o, static_cast<decltype(a) &&>(a)..., b, rest...);
            }});
    else if constexpr (std::is_invocable_r_v<void, B, void (*)(...)>)
      b(_o_{[&](int err) { o(err); },
            [&, ... rest = static_cast<Rest &&>(rest)](auto &&... b) {
              this->pith(o, a, static_cast<decltype(b) &&>(b)..., rest...);
            }});
    else
      pith(static_cast<O &&>(o), a, b, static_cast<Rest &&>(rest)...);
  }
};

template <typename Pith, typename R>
constexpr auto operator^(purry<Pith> l, R &&r) {
  return purry<Pith, R>{l, static_cast<R &&>(r)};
}
template <typename Pith, typename A, typename R>
constexpr auto operator^(purry<Pith, A> l, R &&r) {
  return purry<Pith, A, R>{l.pith, l.a, static_cast<R &&>(r)};
}
template <typename Pith, typename A, typename B, typename R>
constexpr auto operator^(purry<Pith, A, B> l, R &&r) {
  return purry<purry<Pith, A, B>, R>{l, static_cast<R &&>(r)};
}
template <typename R, typename... Args>
constexpr auto operator|(purry<Args...> l, R &&r) {
  return purry{static_cast<R &&>(r), l};
}
static void trr() {
  purry{[](auto o, auto a, auto b) { o(a + b); }, [](auto o) { o(3); },
        [](auto o) { o(6); }}([](auto) {});
  auto ll = purry{[](auto o, auto a, auto b) { o(a + b); }} ^ 3 ^ 6;
  ll([](auto...) {});
}
