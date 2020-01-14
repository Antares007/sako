#pragma once
#include "purry.hpp"
#include <functional>

namespace parsec {

constexpr inline auto rays = ::rays{[](error_ray *, int) {}, [](size_t) {}};

template <typename T>
using if_bark_t = std::enable_if_t<
    std::is_invocable_r_v<void, T, decltype(rays), const char *>>;

struct chr {
  bool (*f)(char);
  template <typename O> void operator()(O o, const char *in) const {
    if (f(in[0]))
      o(1);
    else
      o(error_ray_v, -1);
  }
};

/// 0xxxxxxx
/// 110xxxxx	10xxxxxx
/// 1110xxxx	10xxxxxx	10xxxxxx
/// 11110xxx	10xxxxxx	10xxxxxx	10xxxxxx
struct u8cp {
  bool (*f)(uint32_t);
  template <typename O> void operator()(O o, const char *in) const {
    if ((in[0] & 0x80) == 0)
      if (f(in[0]))
        o(1);
      else
        o(error_ray_v, -1);
    else if ((in[1] & 0xc0) != 0x80)
      o(error_ray_v, -2);
    else if ((in[0] & 0xe0) == 0xc0)
      if (f((0x1f & in[0]) << 6 | (0x3f & in[1])))
        o(2);
      else
        o(error_ray_v, -3);
    else if ((in[2] & 0xc0) != 0x80)
      o(error_ray_v, -4);
    else if ((in[0] & 0xf0) == 0xe0)
      if (f((0x0f & in[0]) << 12 | (0x3f & in[1]) << 6 | (0x3f & in[2])))
        o(3);
      else
        o(error_ray_v, -5);
    else if ((in[3] & 0xc0) != 0x80)
      o(error_ray_v, -6);
    else if ((in[0] & 0xf8) == 0xf0)
      if (f((0x07 & in[0]) << 18 | (0x3f & in[1]) << 12 | (0x3f & in[2]) << 6 |
            (0x3f & in[3])))
        o(4);
      else
        o(error_ray_v, -7);
    else
      o(error_ray_v, -8);
  }
};

struct str {
  const char *match;
  template <typename O> void operator()(O o, const char *in) const {
    size_t i = 0;
    while (char c = match[i])
      if (in[i++] != c)
        return o(error_ray_v, -1);
    o(i);
  }
};

struct noneOf {
  const char *chars;
  template <typename O> void operator()(O o, const char *in) const {
    size_t i = 0;
    while (char c = chars[i++])
      if (in[0] == c)
        return o(error_ray_v, -1);
    o(1);
  }
};

struct anyOf {
  const char *chars;
  template <typename O> void operator()(O o, const char *in) const {
    size_t i = 0;
    while (char c = chars[i++])
      if (in[0] == c)
        return o(1);
    o(error_ray_v, -1);
  }
};

struct till {
  const char *match;
  template <typename O> void operator()(O o, const char *in) const {
    size_t i = 0, j = 0;
    for (;;) {
      char c = match[i], l = in[j];
      if (c == '\0')
        return o(j);
      if (l == '\0')
        return o(error_ray_v, -1);
      if (c == l)
        i++;
      else
        i = match[0] == l ? 1 : 0;
      j++;
    }
  }
};

template <typename P> struct many {
  size_t n = 0;
  P parser;
  template <typename O>
  void operator()(O o, const char *in, size_t acc = 0, size_t count = 0) const {
    parser(::rays{[&](error_ray *, int err) {
                    if (count < n)
                      o(error_ray_v, err);
                    else
                      o(acc);
                  },
                  [&](size_t len) {
                    this->operator()(o, in + len, acc + len, count + 1);
                  }},
           in);
  }
};
template <typename P> many(size_t, P) -> many<P>;

template <typename Parser> struct opt {
  Parser parser;
  template <typename O> void operator()(O o, const char *in) const {
    parser(
        ::rays{[&o](error_ray *, int) { o(0); }, [&o](size_t len) { o(len); }},
        in);
  }
};
template <typename Parser> opt(Parser) -> opt<Parser>;

template <typename L, typename R> struct or_ {
  L l;
  R r;
  template <typename O> void operator()(O o, const char *in) const {
    auto fwdlen = [&](size_t len) { o(len); };
    l(::rays{fwdlen,
             [&](error_ray *, int) {
               r(::rays{fwdlen,
                        [&](error_ray *, int err) { o(error_ray_v, err); }},
                 in);
             }},
      in);
  }
};
template <typename L, typename R> or_(L, R) -> or_<L, R>;

template <typename L, typename R> struct and_ {
  L l;
  R r;
  template <typename O> void operator()(O o, const char *in) const {
    auto fwderr = [&](error_ray *, int err) { o(error_ray_v, err); };
    l(::rays{fwderr,
             [&](size_t llen) {
               r(::rays{fwderr, [&](size_t rlen) { o(llen + rlen); }},
                 in + llen);
             }},
      in);
  }
};
template <typename L, typename R> and_(L, R) -> and_<L, R>;

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
