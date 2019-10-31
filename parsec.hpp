#pragma once
#include "_o_.hpp"
#include "m.hpp"
#include <functional>
namespace next {
struct c {};

template <typename O> void u8cp(const O &o, const char *s, size_t avail) {
  if (1 > avail)
    o(-1);
  else if (0x00 == (0x80 & s[0]))
    o(1, s[0]);
  else if (2 > avail)
    o(-1);
  else if (0xc0 == (0xe0 & s[0]))
    o(2, ((0x1f & s[0]) << 6) | (0x3f & s[1]));
  else if (3 > avail)
    o(-1);
  else if (0xe0 == (0xf0 & s[0]))
    o(3, ((0x0f & s[0]) << 12) | ((0x3f & s[1]) << 6) | (0x3f & s[2]));
  else if (4 > avail)
    o(-1);
  else if (0xf0 == (0xf8 & s[0]))
    o(4, ((0x07 & s[0]) << 18) | ((0x3f & s[1]) << 12) | ((0x3f & s[2]) << 6) |
             (0x3f & s[3]));
  else
    o(-1);
};

template <uint32_t C, uint32_t... Cs> struct chr {
  static constexpr bool hasU8cp = ((C > 127) || ... || (Cs > 127));
  template <typename O>
  void operator()(const O &o, const char *in, const size_t size) const {
    if constexpr (hasU8cp)
      u8cp(_o_{o,
               [&](int s, uint32_t cp) {
                 o(((C == cp) || ... || (Cs == cp)) ? s : -1);
               }},
           in, size);
    else
      o(size > 0 && ((C == in[0]) || ... || (Cs == in[0])) ? 1 : -1);
  }
};

template <uint32_t...> struct rng;
template <uint32_t A, uint32_t B> struct rng<A, B> {
  template <typename O>
  void operator()(const O &o, const char *in, const size_t size) const {
    if constexpr (A > 127 || B > 127)
      u8cp(_o_{o, [&](int s, uint32_t cp) { o(A <= cp && cp <= B ? s : -1); }},
           in, size);
    else
      o(size > 0 && A <= in[0] && in[0] <= B ? 1 : -1);
  }
};
template <uint32_t A, uint32_t B, uint32_t... Rest> struct rng<A, B, Rest...> {
  template <typename O>
  void operator()(const O &o, const char *in, size_t size) const {
    rng<A, B>{}(
        [&](int x) {
          if (x < 0)
            rng<Rest...>{}(o, in, size);
          else
            o(x);
        },
        in, size);
  }
};
template <typename A, typename B> struct or_ {
  A a;
  B b;
  MO()(const char *in, size_t avail) {
    a(
        [&](int x) {
          if (x < 0)
            b(o, in, avail);
          else
            o(x);
        },
        in, avail);
  };
};
template <typename A, typename B> struct and_ {
  A a;
  B b;
  MO()(const char *in, size_t avail) {
    a(
        [&](int x) {
          if (x < 0)
            o(x);
          else
            b([&](int x2) { o(x2 < 0 ? x2 : x2 + x); }, in + x, avail - x);
        },
        in, avail);
  };
};
template <typename A> struct many {
  A a;
  MO()(const char *in, size_t avail, size_t acc = 0) {
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
template <typename A> struct opt {
  A a;
  MO()(const char *in, size_t avail) {
    a([&](int x) { o(x < 0 ? 0 : x); }, in, avail);
  }
};

static auto t() {
  auto x = chr<'\t'>{};
  return x;
}
} // namespace next

namespace parsec {
  template <typename T>
  using is_bark = std::is_invocable_r<void, T, ray<>, const char *, size_t>;

  struct str {
    const char *str_;
    MO()(const char *in, size_t avail) {
      int i = 0;
      while (char c = str_[i]) {
        if (c != in[i] || avail <= static_cast<size_t>(i))
          return o(-1);
        i++;
      }
      o(i);
    }
  };
  UFB(u8cp);
  MO()(const char *s, size_t avail) { ///
    if (avail < 1)
      return o(-1);
    if (0x00 == (0x80 & s[0])) {
      if (a(s[0]))
        o(1);
      else
        o(-1);
    } else if (0xc0 == (0xe0 & s[0])) {
      if (a(((0x1f & s[0]) << 6) | (0x3f & s[1])))
        o(2);
      else
        o(-1);
    } else if (0xe0 == (0xf0 & s[0])) {
      if (a(((0x0f & s[0]) << 12) | ((0x3f & s[1]) << 6) | (0x3f & s[2])))
        o(3);
      else
        o(-1);
    } else if (0xf0 == (0xf8 & s[0])) {
      if (a(((0x07 & s[0]) << 18) | ((0x3f & s[1]) << 12) |
            ((0x3f & s[2]) << 6) | (0x3f & s[3])))
        o(4);
      else
        o(-1);
    } else
      o(-1);
  };
  UFE(u8cp, std::is_convertible<decltype(std::declval<A>()(9)), bool>);
  UFB(chr);
  MO()(const char *in, size_t avail) {
    if (0 < avail && a(in[0]))
      o(1);
    else
      o(-1);
  };
  UFE(chr, std::is_convertible<decltype(std::declval<A>()(' ')), bool>);
  BFB(minus);
  MO()(const char *in, size_t avail) {
    a(
        [&](int x) {
          if (x < 0)
            o(x);
          else
            b([&](int x_) { o(x < 0 ? x_ : -1); }, in, x);
        },
        in, avail);
  };
  BFE(minus, is_bark<A>);
  BFB(or_);
  MO()(const char *in, size_t avail) {
    a(
        [&](int x) {
          if (x < 0)
            b(o, in, avail);
          else
            o(x);
        },
        in, avail);
  };
  BFE(or_, is_bark<A>, is_bark<B>);
  BFB(and_);
  MO()(const char *in, size_t avail) {
    a(
        [&](int x) {
          if (x < 0)
            o(x);
          else
            b([&](int x2) { o(x2 < 0 ? x2 : x2 + x); }, in + x, avail - x);
        },
        in, avail);
  };
  BFE(and_, is_bark<A>, is_bark<B>);
  UFB(many);
  MO()(const char *in, size_t avail, size_t acc = 0) {
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
  UFE(many, is_bark<A>);
  UFB(one_or_many);
  MO()(const char *in, size_t avail) {
    a(
        [&](int x) {
          if (x < 0)
            o(x);
          else {
            many(a)(o, in + x, avail - x, x);
          }
        },
        in, avail);
  }
  UFE(one_or_many, is_bark<A>);
  UFB(opt);
  MO()(const char *in, size_t avail) {
    a([&](int x) { o(x < 0 ? 0 : x); }, in, avail);
  }
  UFE(opt, std::is_invocable_r<void, A, ray<>, const char *, size_t>);
} // namespace parsec

namespace parsec { // operators
template <typename T>
using if_bark_t = std::enable_if_t<
    std::is_invocable_r_v<void, T, ray<>, const char *, size_t>>;

template <typename T> using is_ccharptr = std::is_convertible<T, const char *>;

template <typename L, typename R, typename = if_bark_t<L>,
          typename = if_bark_t<R>>
constexpr auto operator-(L &&l, R &&r) {
  return minus(std::forward<L>(l), std::forward<R>(r));
}
template <typename L, typename R, typename = if_bark_t<L>,
          typename = if_bark_t<R>>
constexpr auto operator|(L &&l, R &&r) {
  return or_(std::forward<L>(l), std::forward<R>(r));
}
template <typename L, typename = if_bark_t<L>>
constexpr auto operator|(L &&l, const char *r) {
  return or_(std::forward<L>(l), str{r});
}
template <typename L, typename R, typename = if_bark_t<L>,
          typename = if_bark_t<R>>
constexpr auto operator&(L &&l, R &&r) {
  return and_(std::forward<L>(l), std::forward<R>(r));
}
template <typename T, typename = if_bark_t<T>>
constexpr auto operator&(const char *l, T &&r) {
  return and_(str{l}, std::forward<T>(r));
}
template <typename T, typename = if_bark_t<T>>
constexpr auto operator&(T &&l, const char *r) {
  return and_(std::forward<T>(l), str{r});
}
} // namespace parsec
