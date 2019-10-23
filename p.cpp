#include "_o_.hpp"
#include "utf8.hpp"
#include <functional>

template <typename... Ts> struct print;

namespace parse {

template <typename T, typename = void> ///
struct par;

template <> ///
struct par<const char *> {
  const char *str;
  template <typename O> void operator()(const char *in, const O &o) const {
    int i = 0;
    while (char c = str[i]) {
      if (c != in[i])
        return o(-1);
      i++;
    }
    o(i);
  }
};

template <typename F> ///
struct par<F, std::enable_if_t<std::is_convertible_v<
                  decltype(std::declval<F>()(utf8::codepoint(""))), bool>>> {
  F f;
  template <typename O> void operator()(const char *in, const O &o) const { ///
    if (in && f(utf8::codepoint(in)))
      o(utf8::next(in) - in);
    else
      o(-1);
  };
};
constexpr inline auto iray = [](int) {};
template <typename Pith> ///
struct par<Pith, std::enable_if_t<std::is_invocable_r_v<
                     void, Pith, const char *, decltype(iray)>>> {
  Pith pith;
  template <typename O> void operator()(const char *in, const O &o) const {
    int next = 0;
    pith(in, [&next](int x) {
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
template <typename F> par(F)->par<F>;

template <typename L, typename R> auto operator|(par<L> l, par<R> r) {
  return par{[=](const char *in, const auto &o) {
    l(in, [&](int x) {
      if (x < 0)
        r(in, o);
      else
        o(x);
    });
  }};
}

template <typename L, typename R> auto operator&(par<L> l, par<R> r) {
  return par{[=](const char *in, const auto &o) {
    l(in, [&](int x) {
        o(x);
        if (0 <= x)
          r(in + x, o);
    });
  }};
}

template <typename P> struct many {
  par<P> p;
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

template <typename P> auto one_or_many(par<P> p) {
  return par{[=]<typename O>(const char *in, const O &o) {
    p(in, [&](int x) {
      o(x);
      if (0 <= x)
        many{p}(in + x, o);
    });
  }};
};

} // namespace parse

namespace parse::xml {
inline auto Char = par{[](uint32_t c) {
  return c == 0x9 || c == 0xA || c == 0xD || (0x20 <= c && c <= 0xD7FF) ||
         (0xE000 <= c && c <= 0xFFFD) || (0x10000 <= c && c <= 0x10FFFF);
}};
inline auto S = one_or_many(par{[](uint32_t c) {
  // S	   ::=   	(#x20 | #x9 | #xD | #xA)+
  return c == 0x20 || c == 0x9 || c == 0xD || c == 0xA;
}});
} // namespace parse::xml

#include "zip.hpp"
#include <fstream>
#include <iostream>
#include <tuple>

static void t() {
  using namespace parse;
  auto logger = [](const char *abv) {
    return [=](int x) {
      if (x < 0)
        std::cout << "error\n";
      else
        std::cout << "[" << std::string_view(abv, x) << "] [" << abv + x
                  << "]\n";
    };
  };
  one_or_many(xml::Char)("01!`ა\001ბAB", logger("01!`ა\001ბAB"));

  auto abv = "აბვ";
  par{"ა"}(abv, logger(abv));

  par{many{par{"A"} | par{"B"} | par{"C"}}}("ACBABABAB", logger("ACBABABAB"));
}

auto main() -> int {
  std::string_view sv = "abc";
  if (sv == "abc")
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
