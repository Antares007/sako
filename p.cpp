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
    size_t i = 0;
    while (char c = str[i]) {
      if (c != in[i])
        return o(-1);
      i++;
    }
    o(std::string_view(in, i), in + i);
  }
};

template <typename F> ///
struct par<F, std::enable_if_t<std::is_convertible_v<
                  decltype(std::declval<F>()(utf8::codepoint(""))), bool>>> {
  F f;
  template <typename O> void operator()(const char *in, const O &o) const { ///
    if (in && f(utf8::codepoint(in))) {
      auto n = utf8::next(in);
      o(std::string_view(in, n - in), n);
    } else
      o(-1);
  };
};

constexpr inline auto void_ray = _o_{[](int) {}, [](auto, const char *) {}};
using void_ray_t = decltype(void_ray);

template <typename Pith> ///
struct par<Pith, std::enable_if_t<std::is_invocable_r_v<
                     void, Pith, const char *, void_ray_t>>> {
  Pith pith;
  template <typename O>
  void operator()(const char *in, const O &o) const {
    pith(in, o);
  };
};
template <typename F> par(F)->par<F>;

template <typename L, typename R> auto operator|(par<L> l, par<R> r) {
  return par{[=](const char *in, const auto &o) {
    l(in, _o_{[&](int) { r(in, o); },
              [&](auto &&m, const char *in) {
                o(std::forward<decltype(m)>(m), in);
              }});
  }};
}

template <typename L, typename R> auto operator&(par<L> l, par<R> r) {
  return par{[=](const char *in, const auto &o) {
    l(in, _o_{[&](int err) { o(err); },
              [&]<typename L_>(L_ &&l, const char *rest) {
                r(rest, _o_{[&](int err) { o(err); },
                            [&, l = std::forward<L_>(l)]<typename R_>(
                                R_ &&r, const char *rest) {
                              o(std::pair{l, std::forward<R_>(r)}, rest);
                            }});
              }});
  }};
}
template <typename P> struct rec {
  par<P> p;
  template <typename O> void operator()(const char *in, const O &o) const {
    p(in, _o_{[&](int) { o("", in); },
              [&](auto m, const char *in) {
                o(m, in);
                this->operator()(in, o);
              }});
  }
};
template <typename P> rec(P)->rec<P>;

template <typename P> auto many(par<P> p) {
  return par{[=]<typename O>(const char *in, const O &o) {
    p(in, _o_{[&](int) { o("", in); },
              [&](auto &&u, const char *in) {
                auto list = std::vector<std::decay_t<decltype(u)>>();
                list.push_back(std::forward<decltype(u)>(u));
                rec{p}(in, [&](auto &&u, const char *in_) {
                  in = in_;
                  list.push_back(std::forward<decltype(u)>(u));
                });
                o(list, in);
              }});
  }};
};
template <typename P> auto one_or_many(par<P> p) {
  return par{[=]<typename O>(const char *in, const O &o) {
    p(in, _o_{[&](int err) { o(err); },
              [&](auto &&u, const char *in) {
                auto list = std::vector<std::decay_t<decltype(u)>>();
                list.push_back(std::forward<decltype(u)>(u));
                rec{p}(in, [&](auto &&u, const char *in_) {
                  in = in_;
                  list.push_back(std::forward<decltype(u)>(u));
                });
                o(list, in);
              }});
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
  auto p1 = par{""};
  auto p3 = par{[](const char *, auto o) {
    o(1);
    o("hi", "hello");
  }};
  (p1 & p1 | p3)("", _o_{[](int) { ///
                         },
                         [](std::pair<std::string_view, std::string_view>,
                            const char *) { ///
                         },
                         [](std::string_view, const char *) { ///

                         }});
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

  parse::one_or_many(parse::xml::Char)(
      "01!`ა\001ბAB", _o_{[](int) { std::cout << "error!\n"; },
                          [](const auto &vec, const char *rest) {
                            std::cout << '[';
                            for (auto n : vec)
                              std::cout << n;
                            std::cout << "] [" << rest << "]\n";
                          }});
  parse::par{"ა"}("აბვ", _o_{[](int) { std::cout << "error\n"; },
                             [](std::string_view f, const char *rest) {
                               std::cout << '[' << f << "] [" << rest << "]\n";
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
