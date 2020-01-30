#pragma once
#define Forward(x) static_cast<decltype(x) &&>(x)
template <class... Ts> struct rays : Ts... { using Ts::operator()...; };
template <class... Ts> rays(Ts...) -> rays<Ts...>;

struct error_ray;
constexpr inline error_ray *error_ray_v = nullptr;

template <typename...> struct curry;
template <typename... Args> curry(Args...) -> curry<Args...>;
template <typename B, typename A> struct curry<B, A> {
  B b;
  A a;
  constexpr curry(auto &&b, auto &&a) : b(Forward(b)), a(Forward(a)) {}
  void operator()(const auto &o, auto &&... rest) const {
    b(o, a, Forward(rest)...);
  }
};
template <typename B, typename A, typename... Tail>
struct curry<B, A, Tail...> : curry<curry<B, A>, Tail...> {
  using curry<curry<B, A>, Tail...>::operator();
  constexpr curry(auto &&b, auto &&a, auto &&... tail)
      : curry<curry<B, A>, Tail...>(curry<B, A>(Forward(b), Forward(a)),
                                    Forward(tail)...) {}
};

template <typename...> struct purry;
template <typename... Args> purry(Args...) -> purry<Args...>;
template <typename B, typename A> struct purry<B, A> {
  B b;
  A a;
  constexpr purry(auto &&b, auto &&a) : b(Forward(b)), a(Forward(a)) {}
  void operator()(const auto &o, auto &&... rest) const {
    a(rays{[&o](error_ray *l, auto &&... rest) { o(l, Forward(rest)...); },
           [&o, this, ... rest = Forward(rest)](auto &&... a) {
             this->b(o, Forward(a)..., rest...);
           }});
  }
};
template <typename B, typename A, typename... Tail>
struct purry<B, A, Tail...> : purry<purry<B, A>, Tail...> {
  using purry<purry<B, A>, Tail...>::operator();
  constexpr purry(auto &&b, auto &&a, auto &&... tail)
      : purry<purry<B, A>, Tail...>(purry<B, A>(Forward(b), Forward(a)),
                                    Forward(tail)...) {}
};
