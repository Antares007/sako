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

int main() {
  auto text = "g0t0<grammar::E41::E, grammar::E41::E>";

  constexpr static const char abcაბვ[] = "abcაბვ";
  using p = str<abcაბვ>;

  p{}([](auto len) { std::cout << len << '\n'; }, text);

  return 9;
}
