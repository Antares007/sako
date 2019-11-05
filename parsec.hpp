#pragma once
#include "_o_.hpp"
#include "m.hpp"
#include <functional>
namespace parsec {

/// 0xxxxxxx
/// 110xxxxx	10xxxxxx
/// 1110xxxx	10xxxxxx	10xxxxxx
/// 11110xxx	10xxxxxx	10xxxxxx	10xxxxxx
template <typename F,
          typename = std::enable_if_t<std::is_invocable_r_v<bool, F, uint32_t>>>
constexpr inline auto u8cp(F &&f) {
  return [f = static_cast<F &&>(f)](auto o, const char *in, size_t a) {
    if (a < 1)
      o(-1);
    else if ((in[0] & 0x80) == 0)
      if (f(in[0]))
        o(1);
      else
        o(-1);
    else if (a < 2 || (in[1] & 0xc0) != 0x80)
      o(-2);
    else if ((in[0] & 0xe0) == 0xc0)
      if (f((0x1f & in[0]) << 6 | (0x3f & in[1])))
        o(2);
      else
        o(-1);
    else if (a < 3 || (in[2] & 0xc0) != 0x80)
      o(-3);
    else if ((in[0] & 0xf0) == 0xe0)
      if (f((0x0f & in[0]) << 12 | (0x3f & in[1]) << 6 | (0x3f & in[2])))
        o(3);
      else
        o(-1);
    else if (a < 4 || (in[3] & 0xc0) != 0x80)
      o(-4);
    else if ((in[0] & 0xf8) == 0xf0)
      if (f((0x07 & in[0]) << 18 | (0x3f & in[1]) << 12 | (0x3f & in[2]) << 6 |
            (0x3f & in[3])))
        o(4);
      else
        o(-1);
    else
      o(-5);
  };
};
constexpr inline auto str(const char *str_) {
  return [=](auto o, const char *in, size_t avail) {
    int i = 0;
    while (char c = str_[i]) {
      if (c != in[i] || avail <= static_cast<size_t>(i))
        return o(-1);
      i++;
    }
    o(i);
  };
};

template <typename T>
using if_bark_t = std::enable_if_t<
    std::is_invocable_r_v<void, T, ray<>, const char *, size_t>>;

template <typename A> struct many0 {
  A a;
  template <typename U, typename = if_bark_t<U>>
  constexpr explicit many0<U>(U &&u) : a(static_cast<U &&>(u)) {}
  M()(const char *in, size_t avail, size_t acc = 0) {
    a(
        [&](int x) {
          if (x < 0)
            o(acc);
          else {
            this->operator()(o, in + x, avail - x, acc + x);
          }
        },
        in, avail);
  }
};
template <typename A> many0(A)->many0<A>;

template <typename A> struct many1 {
  A a;
  template <typename U, typename = if_bark_t<U>>
  constexpr explicit many1<U>(U &&u) : a(static_cast<U &&>(u)) {}
  M()(const char *in, size_t avail, size_t acc = -1) {
    a(
        [&](int x) {
          if (x < 0)
            o(acc);
          else {
            this->operator()(o, in + x, avail - x,
                             (acc == static_cast<size_t>(-1) ? x : acc + x));
          }
        },
        in, avail);
  }
};
template <typename A> many1(A)->many1<A>;

template <typename P, typename = if_bark_t<P>>
constexpr inline auto opt(P &&p) {
  return [p = static_cast<P &&>(p)](auto o, const char *in, size_t avail) {
    p([&](int x) { o(x < 0 ? 0 : x); }, in, avail);
  };
};

template <uint32_t C, uint32_t... Cs> constexpr inline auto anyof() {
  return [](auto o, const char *in, size_t avail) {
    constexpr bool hasU8cp = ((C > 127) || ... || (Cs > 127));
    if constexpr (hasU8cp)
      u8cp([](auto cp) { return ((C == cp) || ... || (Cs == cp)); })(o, in,
                                                                     avail);

    else
      o(avail > 0 && ((C == in[0]) || ... || (Cs == in[0])) ? 1 : -1);
  };
};

template <uint32_t C, uint32_t... Cs> constexpr inline auto noneof() {
  return [](auto o, const char *in, size_t avail) {
    u8cp([](auto cp) { return ((C != cp) && ... && (Cs != cp)); })(o, in,
                                                                   avail);
  };
};

template <typename L, typename R, typename = if_bark_t<L>,
          typename = if_bark_t<R>>
constexpr auto operator|(L &&l, R &&r) {
  return [a = static_cast<L &&>(l),
          b = static_cast<R &&>(r)](auto o, const char *in, size_t avail) {
    a(
        [&](int x) {
          if (x < 0)
            b(o, in, avail);
          else
            o(x);
        },
        in, avail);
  };
}
template <typename L, typename = if_bark_t<L>>
constexpr auto operator|(L &&l, const char *r) {
  return static_cast<L &&>(l) | str(r);
}
template <typename R, typename = if_bark_t<R>>
constexpr auto operator|(const char *l, R &&r) {
  return str(l) | static_cast<R &&>(r);
}

template <typename L, typename R, typename = if_bark_t<L>,
          typename = if_bark_t<R>>
constexpr auto operator&(L &&l, R &&r) {
  return [a = static_cast<L &&>(l),
          b = static_cast<R &&>(r)](auto o, const char *in, size_t avail) {
    a(
        [&](int xa) {
          if (xa < 0)
            o(xa);
          else
            b([&](int xb) { o(xb < 0 ? xb : xa + xb); }, in + xa, avail - xa);
        },
        in, avail);
  };
}
template <typename L, typename = if_bark_t<L>>
constexpr auto operator&(L &&l, const char *r) {
  return static_cast<L &&>(l) & str(r);
}
template <typename R, typename = if_bark_t<R>>
constexpr auto operator&(const char *l, R &&r) {
  return str(l) & static_cast<R &&>(r);
}

} // namespace parsec
