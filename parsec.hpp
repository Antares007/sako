#pragma once
#include "_o_.hpp"
#include "m.hpp"
#include <functional>

namespace parsec {
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
    if (a(((0x07 & s[0]) << 18) | ((0x3f & s[1]) << 12) | ((0x3f & s[2]) << 6) |
          (0x3f & s[3])))
      o(4);
    else
      o(-1);
  } else
    o(-1);
};
UFE(u8cp, std::is_convertible<decltype(std::declval<A>()(9)), bool>);
UFB(chr);
MO()(const char *in, size_t avail) { ///
  if (0 < avail && a(in[0]))
    o(1);
  else
    o(-1);
};
UFE(chr, std::is_convertible<decltype(std::declval<A>()(' ')), bool>);
template <typename T>
using if_bark_t = std::enable_if_t<
    std::is_invocable_r_v<void, T, const char *, size_t, ray<>>>;
BFB(minus);
MO()(const char *in, size_t avail) { ///
  a(in, avail, [&](int x) {
    if (x < 0)
      o(x);
    else
      b(in, x, [x_ = x, &o](int x) { o(x < 0 ? x_ : -1); });
  });
};
BFE(minus, std::is_invocable_r<void, A, const char *, size_t, ray<>>);

template <typename L, typename R, typename = if_bark_t<L>,
          typename = if_bark_t<R>>
constexpr auto operator-(L &&l, R &&r) {
  return minus_fn<L, R>{std::forward<L>(l), std::forward<R>(r)};
}
BFB(or_);
MO()(const char *in, size_t avail) { ///
  a(in, avail, [&](int x) {
    if (x < 0)
      b(in, avail, o);
    else
      o(x);
  });
};
BFE(or_, std::is_invocable_r<void, A, const char *, size_t, ray<>>,
    std::is_invocable_r<void, B, const char *, size_t, ray<>>);

template <typename L, typename R, typename = if_bark_t<L>,
          typename = if_bark_t<R>>
constexpr auto operator|(L &&l, R &&r) {
  return or_(std::forward<L>(l), std::forward<R>(r));
}
template <typename L, typename = if_bark_t<L>>
constexpr auto operator|(L &&l, const char *r) {
  return or_(std::forward<L>(l), str{r});
}
BFB(and_);
MO()(const char *in, size_t avail) { ///
  a(in, avail, [&](int x) {
    if (x < 0)
      o(x);
    else
      b(in + x, avail - x, [&](int x2) { o(x2 < 0 ? x2 : x2 + x); });
  });
};
BFE(and_, std::is_invocable_r<void, A, const char *, size_t, ray<>>,
    std::is_invocable_r<void, B, const char *, size_t, ray<>>);

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
UFB(many);
template <typename O>
void operator()(const char *in, size_t avail, const O &o,
                size_t acc = 0) const {
  a(in, avail, [&](int x) {
    if (x < 0)
      o(acc);
    else {
      this->operator()(in + x, avail - x, o, acc + x);
    }
  });
}
UFE(many, std::is_invocable_r<void, A, const char *, size_t, ray<>>);
UFB(one_or_many);
MO()(const char *in, size_t avail) {
  a(in, avail, [&](int x) {
    if (x < 0)
      o(x);
    else {
      many(a)(in + x, avail - x, o, x);
    }
  });
}
UFE(one_or_many, std::is_invocable_r<void, A, const char *, size_t, ray<>>);
UFB(opt);
MO()(const char *in, size_t avail) {
  a(in, avail, [&](int x) { o(x < 0 ? 0 : x); });
}
UFE(opt, std::is_invocable_r<void, A, const char *, size_t, ray<>>);
} // namespace parsec

