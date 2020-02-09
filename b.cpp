#include "lalr/mb.hpp"
#include "purry.hpp"
#include <iostream>

template <typename... Ts> struct print;

template <typename L, typename R> struct or_ {
  L l;
  R r;
  void operator()(Car o, const char *in) const {
    l(
        [&](int len) {
          if (len < 0)
            r(o, in);
          else
            o(len);
        },
        in);
  }
};
template <typename L, typename R> or_(L, R) -> or_<L, R>;

template <typename L, typename R> struct and_ {
  L l;
  R r;
  template <typename O> void operator()(O o, const char *in) const {
    l(
        [&](int len) {
          if (len < 0)
            o(len);
          else
            r(o, in + len);
        },
        in);
  }
};
template <typename L, typename R> and_(L, R) -> and_<L, R>;

template <char... Chrs> struct any {
  void operator()(Car o, const char *b) {
    if constexpr (((Chrs > 0x80) || ...))
      ;
    else
      ;
    o(((Chrs == b[0]) || ...) ? 1 : -1);
  }
};

template <char... Chrs> struct none {
  void operator()(Car o, const char *b) { o(((Chrs != b[0]) && ...) ? 1 : -1); }
};

template <const char *Match> struct str {
  void operator()(Car o, const char *in) const {
    size_t i = 0;
    while (char c = Match[i])
      if (in[i++] != c)
        return o(-1);
    o(i);
  }
};

constexpr inline auto l = [](Car o) {
  o(head_ray_v, 1,
    [&](Car o) { o(head_ray_v, 2, [&](Car o) { o(head_ray_v, 3); }); });
};
struct pith {
  void operator()(head_ray *, int i, Car... tail) const {
    std::cout << i << ' ';
    if constexpr (sizeof...(tail) == 0)
      std::cout << "end!";
    (tail(*this), ...);
  }
};
uint32_t nProcGen = 369;
uint32_t rnd() {
  nProcGen += 0xe120fc15;
  uint64_t tmp;
  tmp = (uint64_t)nProcGen * 0x4a39b70d;
  uint32_t m1 = (tmp >> 32) ^ tmp;
  tmp = (uint64_t)m1 * 0x12fad5c9;
  uint32_t m2 = (tmp >> 32) ^ tmp;
  return m2;
}
int main() {
  l(pith{});
  auto text = "g0t0<grammar::E41::E, grammar::E41::E>";

  constexpr static const char abcაბვ[] = "abcაბვ";
  using p = str<abcაბვ>;

  p{}([](auto len) { std::cout << len << '\n'; }, text);
  for (int i = 0; i < 100; i++)
    std::cout << rnd() % 2 << '\n';
  return 9;
}
