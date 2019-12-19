#pragma once
#include "_o_.hpp"
#include <functional>

namespace parsec {

struct rez_ray;
constexpr inline rez_ray *rez = nullptr;
constexpr inline auto rays = _o_{[](int) {}, [](rez_ray *) {}};

template <typename T>
using if_bark_t = std::enable_if_t<
    std::is_invocable_r_v<void, T, decltype(rays), const char *>>;

template <typename F> struct chr {
  F f;
  template <typename O> void operator()(const O &o, const char *in) const {
    if (in[0] && f(in[0]))
      o(rez, 1);
    else
      o(-1);
  }
};
template <typename F,
          typename = std::enable_if_t<std::is_invocable_r_v<bool, F, char>>>
chr(F)->chr<F>;

/// 0xxxxxxx
/// 110xxxxx	10xxxxxx
/// 1110xxxx	10xxxxxx	10xxxxxx
/// 11110xxx	10xxxxxx	10xxxxxx	10xxxxxx
template <typename F> struct u8cp {
  F f;
  template <typename O> void operator()(const O &o, const char *in) const {
    if (in[0] == 0)
      o(-1);
    else if ((in[0] & 0x80) == 0)
      if (f(in[0]))
        o(rez, 1);
      else
        o(-2);
    else if ((in[1] & 0xc0) != 0x80)
      o(-3);
    else if ((in[0] & 0xe0) == 0xc0)
      if (f((0x1f & in[0]) << 6 | (0x3f & in[1])))
        o(rez, 2);
      else
        o(-4);
    else if ((in[2] & 0xc0) != 0x80)
      o(-5);
    else if ((in[0] & 0xf0) == 0xe0)
      if (f((0x0f & in[0]) << 12 | (0x3f & in[1]) << 6 | (0x3f & in[2])))
        o(rez, 3);
      else
        o(-6);
    else if ((in[3] & 0xc0) != 0x80)
      o(-7);
    else if ((in[0] & 0xf8) == 0xf0)
      if (f((0x07 & in[0]) << 18 | (0x3f & in[1]) << 12 | (0x3f & in[2]) << 6 |
            (0x3f & in[3])))
        o(rez, 4);
      else
        o(-8);
    else
      o(-9);
  }
};
template <typename F,
          typename = std::enable_if_t<std::is_invocable_r_v<bool, F, uint32_t>>>
u8cp(F)->u8cp<F>;

struct str {
  const char *match;
  template <typename O> void operator()(const O &o, const char *in) const {
    int i = 0;
    while (char c = match[i]) {
      if (c != in[i])
        return o(-1);
      i++;
    }
    o(rez, i);
  }
};

template <typename Parser> struct many0 {
  Parser parser;
  template <typename O>
  void operator()(const O &o, const char *in, size_t acc = 0) {
    parser(_o_{[&](int) { o(rez, acc); },
               [&](rez_ray *, int len) {
                 this->operator()(o, in + len, acc + len);
               }},
           in);
  }
};
template <typename Parser> many0(Parser)->many0<Parser>;

template <typename Parser> struct opt {
  Parser parser;
  template <typename O> void operator()(const O &o, const char *in) {
    parser(
        _o_{[&](int) { o(rez, 0); }, [&](rez_ray *, int len) { o(rez, len); }},
        in);
  }
};
template <typename Parser> opt(Parser)->opt<Parser>;

template <typename L, typename R> struct or_ {
  L l;
  R r;
  template <typename O> void operator()(const O &o, const char *in) {
    l(_o_{[&](int) {
            r(_o_{[&](int err) { o(err); },
                  [&](rez_ray *, int len) { o(rez, len); }},
              in);
          },
          [&](rez_ray *, int len) { o(rez, len); }},
      in);
  }
};
template <typename L, typename R> or_(L, R)->or_<L, R>;

template <typename L, typename R> struct and_ {
  L l;
  R r;
  template <typename O> void operator()(const O &o, const char *in) {
    l(_o_{[&](int err) { o(err); },
          [&](rez_ray *, int llen) {
            r(_o_{[&](int err) { o(err); },
                  [&](rez_ray *, int rlen) { o(rez, llen + rlen); }},
              in + llen);
          }},
      in);
  }
};
template <typename L, typename R> and_(L, R)->and_<L, R>;

template <typename L, typename R, typename = if_bark_t<L>,
          typename = if_bark_t<R>>
constexpr auto operator|(L &&l, R &&r) {
  return or_<L, R>{static_cast<L &&>(l), static_cast<R &&>(r)};
}

template <typename L, typename R, typename = if_bark_t<L>,
          typename = if_bark_t<R>>
constexpr auto operator&(L &&l, R &&r) {
  return and_<L, R>{static_cast<L &&>(l), static_cast<R &&>(r)};
}

} // namespace parsec
