#pragma once
#define F(x) static_cast<decltype(x) &&>(x)
template <class... Ts> struct rays : Ts... { using Ts::operator()...; };
template <class... Ts> rays(Ts...) -> rays<Ts...>;

struct error_ray;
constexpr inline error_ray *error_ray_v = nullptr;

template <typename...> struct curry;
template <typename... Args> curry(Args...) -> curry<Args...>;
template <typename B, typename A> struct curry<B, A> {
  B b;
  A a;
  constexpr curry(auto &&b, auto &&a) : b(F(b)), a(F(a)) {}
  void operator()(const auto &o, auto &&... rest) const { b(o, a, F(rest)...); }
};
template <typename B, typename A, typename... Tail>
struct curry<B, A, Tail...> : curry<curry<B, A>, Tail...> {
  using curry<curry<B, A>, Tail...>::operator();
  constexpr curry(auto &&b, auto &&a, auto &&... tail)
      : curry<curry<B, A>, Tail...>(curry<B, A>(F(b), F(a)), F(tail)...) {}
};

template <typename...> struct purry;
template <typename... Args> purry(Args...) -> purry<Args...>;
template <typename B, typename A> struct purry<B, A> {
  B b;
  A a;
  constexpr purry(auto &&b, auto &&a) : b(F(b)), a(F(a)) {}
  void operator()(const auto &o, auto &&... rest) const {
    a(rays{[&o](error_ray *l, auto &&... rest) { o(l, F(rest)...); },
           [&o, this, ... rest = F(rest)](auto &&... a) {
             this->b(o, F(a)..., rest...);
           }});
  }
};
template <typename B, typename A, typename... Tail>
struct purry<B, A, Tail...> : purry<purry<B, A>, Tail...> {
  using purry<purry<B, A>, Tail...>::operator();
  constexpr purry(auto &&b, auto &&a, auto &&... tail)
      : purry<purry<B, A>, Tail...>(purry<B, A>(F(b), F(a)), F(tail)...) {}
};

struct head_ray;
struct tail_ray;
struct list_pith {
  void operator()(head_ray *, const auto &) const {}
  void operator()(tail_ray *, const auto &tail) const { tail(*this); }
};
template <typename T> concept List = requires(T l) { l(list_pith{}); };

constexpr inline auto head_ray_v = static_cast<head_ray *>(nullptr);
constexpr inline auto tail_ray_v = static_cast<tail_ray *>(nullptr);
template <typename...> struct list;
template <typename... Args> list(Args...) -> list<Args...>;
template <> struct list<> {
  void operator()(const auto &) const {}
};

template <typename H> struct list<H> {
  H h;
  constexpr list(auto &&h) : h(F(h)) {}
  void operator()(const auto &o) const { o(head_ray_v, h); }
};
template <typename H, typename... Tail>
struct list<H, Tail...> : list<Tail...> {
  H h;
  constexpr list(auto &&h, auto &&... tail)
      : list<Tail...>(F(tail)...), h(F(h)) {}
  void operator()(const auto &o) const {
    o(head_ray_v, h);
    o(tail_ray_v, [&](const auto &o) { list<Tail...>::operator()(o); });
  }
};
#undef F
