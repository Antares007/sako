#pragma once
#include "_o_.hpp"
#include <functional>
namespace parsec {
struct str {
  const char *str_;
  constexpr explicit str(const char *rhs) noexcept : str_(rhs) {}
  template <typename O>
  void operator()(const char *in, size_t avail, const O &o) const {
    int i = 0;
    while (char c = str_[i]) {
      if (c != in[i] || avail <= static_cast<size_t>(i))
        return o(-1);
      i++;
    }
    o(i);
  }
};
inline namespace {    /// u8cp
template <typename F> ///
struct u8cp {
  F f;
  template <typename U, typename = std::enable_if_t<std::is_convertible_v<
                            decltype(std::declval<U>()(9)), bool>>>
  constexpr explicit u8cp(U &&u) noexcept : f(std::forward<U>(u)) {}

  template <typename O>
  void operator()(const char *s, size_t avail, const O &o) const { ///
    if (avail < 1)
      return o(-1);
    if (0x00 == (0x80 & s[0])) {
      if (f(s[0]))
        o(1);
      else
        o(-1);
    } else if (0xc0 == (0xe0 & s[0])) {
      if (f(((0x1f & s[0]) << 6) | (0x3f & s[1])))
        o(2);
      else
        o(-1);
    } else if (0xe0 == (0xf0 & s[0])) {
      if (f(((0x0f & s[0]) << 12) | ((0x3f & s[1]) << 6) | (0x3f & s[2])))
        o(3);
      else
        o(-1);
    } else if (0xf0 == (0xf8 & s[0])) {
      if (f(((0x07 & s[0]) << 18) | ((0x3f & s[1]) << 12) |
            ((0x3f & s[2]) << 6) | (0x3f & s[3])))
        o(4);
      else
        o(-1);
    } else
      o(-1);
  };
};
template <typename F> u8cp(F)->u8cp<F>;
} // namespace
inline namespace {    /// chr
template <typename F> ///
struct chr {
  F f;
  template <typename U, typename = std::enable_if_t<std::is_convertible_v<
                            decltype(std::declval<U>()(' ')), bool>>>
  constexpr explicit chr(U &&u) noexcept : f(std::forward<U>(u)) {}

  template <typename O>
  void operator()(const char *in, size_t avail, const O &o) const { ///
    if (0 < avail && f(in[0]))
      o(1);
    else
      o(-1);
  };
};
template <typename F> chr(F)->chr<F>;
} // namespace
inline namespace { /// concepts
template <typename T>
using if_bark_t = std::enable_if_t<
    std::is_invocable_r_v<void, T, const char *, size_t, ray<>>>;
} // namespace
inline namespace {                /// minus
template <typename L, typename R> ///
struct minus {
  L l;
  R r;
  template <typename UL, typename UR, typename = if_bark_t<UL>,
            typename = if_bark_t<UR>>
  constexpr explicit minus(UL &&ul, UR &&ur) noexcept
      : l(std::forward<UL>(ul)), r(std::forward<UR>(ur)) {}
  template <typename O>
  void operator()(const char *in, size_t avail, const O &o) const { ///
    l(in, avail, [&](int x) {
      if (x < 0)
        o(x);
      else
        r(in, x, [x_ = x, &o](int x) { o(x < 0 ? x_ : -1); });
    });
  };
};
template <typename L, typename R> minus(L, R)->minus<L, R>;

template <typename L, typename R, typename = if_bark_t<L>,
          typename = if_bark_t<R>>
constexpr auto operator-(L &&l, R &&r) {
  return minus(std::forward<L>(l), std::forward<R>(r));
}
} // namespace
inline namespace {                /// or
template <typename L, typename R> ///
struct or_ {
  L l;
  R r;
  template <typename UL, typename UR, typename = if_bark_t<UL>,
            typename = if_bark_t<UR>>
  constexpr explicit or_(UL &&ul, UR &&ur) noexcept
      : l(std::forward<UL>(ul)), r(std::forward<UR>(ur)) {}

  template <typename O>
  void operator()(const char *in, size_t avail, const O &o) const { ///
    l(in, avail, [&](int x) {
      if (x < 0)
        r(in, avail, o);
      else
        o(x);
    });
  };
};
template <typename L, typename R> or_(L, R)->or_<L, R>;

template <typename L, typename R, typename = if_bark_t<L>,
          typename = if_bark_t<R>>
constexpr auto operator|(L &&l, R &&r) {
  return or_(std::forward<L>(l), std::forward<R>(r));
}
template <typename L, typename = if_bark_t<L>>
constexpr auto operator|(L &&l, const char *r) {
  return or_(std::forward<L>(l), str{r});
}
} // namespace
inline namespace {                /// and
template <typename L, typename R> ///
struct and_ {
  L l;
  R r;
  template <typename UL, typename UR, typename = if_bark_t<UL>,
            typename = if_bark_t<UR>>
  constexpr explicit and_(UL &&ul, UR &&ur) noexcept

      : l(std::forward<UL>(ul)), r(std::forward<UR>(ur)) {}

  template <typename O>
  void operator()(const char *in, size_t avail, const O &o) const { ///
    l(in, avail, [&](int x) {
      if (x < 0)
        o(x);
      else
        r(in + x, avail - x, [&](int x2) { o(x2 < 0 ? x2 : x2 + x); });
    });
  };
};
template <typename L, typename R> and_(L, R)->and_<L, R>;

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
} // namespace
inline namespace { /// many
template <typename P> struct many {
  P p;
  template <typename U, typename = if_bark_t<U>>
  constexpr explicit many(U &&u) noexcept : p(std::forward<U>(u)) {}

  template <typename O>
  void operator()(const char *in, size_t avail, const O &o,
                  size_t a = 0) const {
    p(in, avail, [&](int x) {
      if (x < 0)
        o(a);
      else {
        this->operator()(in + x, avail - x, o, a + x);
      }
    });
  }
};
template <typename P> many(P)->many<P>;
} // namespace
inline namespace { /// one_or_many
template <typename P> struct one_or_many {
  P p;
  template <typename U, typename = if_bark_t<U>>
  constexpr explicit one_or_many(U &&u) noexcept : p(std::forward<U>(u)) {}

  template <typename O>
  void operator()(const char *in, size_t avail, const O &o) const {
    p(in, avail, [&](int x) {
      if (x < 0)
        o(x);
      else {
        many{p}(in + x, avail - x, o, x);
      }
    });
  }
};
template <typename P> one_or_many(P)->one_or_many<P>;
} // namespace
inline namespace { /// opt
template <typename P> struct opt {
  P p;
  template <typename U, typename = if_bark_t<U>>
  constexpr explicit opt(U &&u) noexcept : p(std::forward<U>(u)) {}

  template <typename O>
  void operator()(const char *in, size_t avail, const O &o) const {
    p(in, avail, [&](int x) { o(x < 0 ? 0 : x); });
  }
};
template <typename P> opt(P)->opt<P>;
} // namespace
} // namespace parsec

