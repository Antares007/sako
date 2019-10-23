#include "_o_.hpp"
#include "utf8.hpp"
#include <functional>

template <typename... Ts> struct print;

namespace parse {

struct str {
  const char *str_;
  explicit str(const char *rhs) noexcept : str_(rhs) {}
  template <typename O> void operator()(const char *in, const O &o) const {
    int i = 0;
    while (char c = str_[i]) {
      if (c != in[i])
        return o(-1);
      i++;
    }
    o(i);
  }
};

template <typename F> ///
struct satisfy {
  F f;
  template <typename U,
            typename = std::enable_if_t<std::is_convertible_v<
                decltype(std::declval<U>()(utf8::codepoint(""))), bool>>>
  explicit satisfy(U &&u) noexcept : f(std::forward<U>(u)) {}

  template <typename O> void operator()(const char *in, const O &o) const { ///
    if (in && f(utf8::codepoint(in)))
      o(utf8::next(in) - in);
    else
      o(-1);
  };
};
template <typename F> satisfy(F)->satisfy<F>;

constexpr inline auto aray = [](auto) {};
using aray_t = decltype(aray);

template <typename T>
using if_bark_t =
    std::enable_if_t<std::is_invocable_r_v<void, T, const char *, aray_t>>;

template <typename P> ///
struct sum {
  P p;
  template <typename U, typename = if_bark_t<U>>
  explicit sum(U &&u) noexcept : p(std::forward<U>(u)) {}

  template <typename O> void operator()(const char *in, const O &o) const { ///
    int next = 0;
    p(in, [&next](int x) {
      if (0 <= next) {
        if (x < 0)
          next = x;
        else
          next += x;
      }
    });
    o(next);
  };
};
template <typename P> sum(P)->sum<P>;

template <typename L, typename R> ///
struct or_ {
  L l;
  R r;
  template <typename UL, typename UR, typename = if_bark_t<UL>,
            typename = if_bark_t<UR>>
  explicit or_(UL &&ul, UR &&ur) noexcept
      : l(std::forward<UL>(ul)), r(std::forward<UR>(ur)) {}

  template <typename O> void operator()(const char *in, const O &o) const { ///
    l(in, [&](int x) {
      if (x < 0)
        r(in, o);
      else
        o(x);
    });
  };
};
template <typename L, typename R> or_(L, R)->or_<L, R>;

template <typename L, typename R, typename = if_bark_t<L>,
          typename = if_bark_t<R>>
auto operator|(L &&l, R &&r) {
  return or_(std::forward<L>(l), std::forward<R>(r));
}

template <typename L, typename R> ///
struct and_ {
  L l;
  R r;
  template <typename UL, typename UR, typename = if_bark_t<UL>,
            typename = if_bark_t<UR>>
  explicit and_(UL &&ul, UR &&ur) noexcept
      : l(std::forward<UL>(ul)), r(std::forward<UR>(ur)) {}

  template <typename O> void operator()(const char *in, const O &o) const { ///
    l(in, [&](int x) {
      o(x);
      if (0 <= x)
        r(in + x, o);
    });
  };
};
template <typename L, typename R> and_(L, R)->and_<L, R>;

template <typename L, typename R, typename = if_bark_t<L>,
          typename = if_bark_t<R>>
auto operator&(L &&l, R &&r) {
  return and_(std::forward<L>(l), std::forward<R>(r));
}

template <typename P> struct many {
  P p;
  template <typename U, typename = if_bark_t<U>>
  explicit many(U &&u) noexcept : p(std::forward<U>(u)) {}

  template <typename O> void operator()(const char *in, const O &o) const {
    p(in, [&](int x) {
      if (x < 0)
        o(0);
      else {
        o(x);
        this->operator()(in + x, o);
      }
    });
  }
};
template <typename P> many(P)->many<P>;

template <typename P> struct one_or_many {
  P p;
  template <typename U, typename = if_bark_t<U>>
  explicit one_or_many(U &&u) noexcept : p(std::forward<U>(u)) {}

  template <typename O> void operator()(const char *in, const O &o) const {
    p(in, [&](int x) {
      o(x);
      if (0 <= x)
        many{p}(in + x, o);
    });
  }
};
template <typename P> one_or_many(P)->one_or_many<P>;

} // namespace parse

namespace parse::xml {
inline auto Char = satisfy{[](uint32_t c) {
  return c == 0x9 || c == 0xA || c == 0xD || (0x20 <= c && c <= 0xD7FF) ||
         (0xE000 <= c && c <= 0xFFFD) || (0x10000 <= c && c <= 0x10FFFF);
}};
inline auto S = one_or_many(satisfy{[](uint32_t c) {
  // S	   ::=   	(#x20 | #x9 | #xD | #xA)+
  return c == 0x20 || c == 0x9 || c == 0xD || c == 0xA;
}});
} // namespace parse::xml

#include "zip.hpp"
#include <fstream>
#include <iostream>
#include <tuple>

namespace parse {
template <size_t... N> struct cp;
template <size_t N> struct cp<N> {
  template <typename O> void operator()(const char *in, const O &o) const { ///
    if (utf8::codepoint(in) == N)
      o(utf8::next(in) - in);
    else
      o(-1);
  };
};
template <size_t S, size_t E> struct cp<S, E> {
  template <typename O> void operator()(const char *in, const O &o) const { ///
    const auto cp = utf8::codepoint(in);
    if (S <= cp && cp <= E)
      o(utf8::next(in) - in);
    else
      o(-1);
  };
};
} // namespace parse

static void t() {
  using namespace parse;
  auto z = cp<'0', '9'>{} | cp<100>{};
  auto run = [](const char *in, const auto &parser) {
    sum{parser}(in, [=](int x) {
      if (x < 0)
        std::cout << "error\n";
      else
        std::cout << "[" << std::string_view(in, x) << "] [" << in + x << "]\n";
    });
  };
  run("01!`ა\001ბAB", one_or_many{xml::Char});

  run("აბვ", str{"ა"});

  run("ACBABABAB", many{str{"A"} | str{"B"} | str{"C"}});
}

auto main() -> int {
  t();
  std::ifstream s("MyTest.xlsx", std::ios::binary);
  s.seekg(0, std::ios_base::end);
  const size_t size = static_cast<std::size_t>(s.tellg());
  std::vector<char> buf;
  buf.reserve(size);
  s.seekg(0);
  s.read(buf.data(), size);

  zip::unzip(reinterpret_cast<const uint8_t *>(buf.data()), size,
             _o_{[](const char *err) { std::cout << err << "\n"; },
                 [](std::string_view name, auto &&p) {
                   p(_o_{[&](auto err) { std::cout << err << "\n"; },
                         [&](auto, auto size) {
                           std::cout << name << " - " << size << "\n";
                         }});
                 }});

  // auto l = pand(pstr("a"), pstr("b"));
  // l("abo", _o_{[](int) { std::cout << "error\n"; },
  //                    [](const char *f, const char *s, const char *rest) {
  //                      std::cout << '[' << f << s << "] [" << rest << "]\n";
  //                    }});

  // auto l2 = por(pstr("a"), pstr("b"));
  // l2("b", _o_{[](int) { std::cout << "error\n"; },
  //                   [](const char *f, const char *rest) {
  //                     std::cout << '[' << f << "] [" << rest << "]\n";
  //                   }});
  //
}
