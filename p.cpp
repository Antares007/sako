#include "_o_.hpp"
#include "utf8.hpp"
#include <functional>

template <typename... Ts> struct print;

namespace parse {
auto str(const char *str) {
  return [=]<typename O>(const char *in, O &&o) {
    size_t i = 0;
    while (char c = str[i]) {
      if (c != in[i])
        return o(-1);
      i++;
    }
    o(str, in + i);
  };
};
template <typename F> auto satisfy(F &&f) noexcept {
  return [f = std::forward<F>(f)]<typename O>(const char *in, O &&o) { ///
    if (in && f(utf8::codepoint(in))) {
      auto n = utf8::next(in);
      o(std::string_view(in, n - in), n);
    } else
      o(-1);
  };
}

template <typename Pl, typename Pr> auto pand(Pl &&pl, Pr &&pr) {
  return [pl = std::forward<Pl>(pl),
          pr = std::forward<Pr>(pr)]<typename O>(const char *in, O &&o) {
    pl(in, _o_{o, [&](const char *v1, const char *rest) {
                 pr(rest, _o_{o, [&](const char *v2, const char *rest) {
                                o(v1, v2, rest);
                              }});
               }});
  };
}

template <typename Pl, typename Pr> auto por(Pl &&pl, Pr &&pr) {
  return [pl = std::forward<Pl>(pl),
          pr = std::forward<Pr>(pr)]<typename O>(const char *in, O &&o) {
    pl(in, _o_{[&](int) { pr(in, o); },
               [&](const char *m, const char *in) { o(m, in); }});
  };
}
template <typename P> struct many {
  P p;
  template <typename O> void operator()(const char *in, O &&o) const {
    p(in, _o_{[](int) {},
              [&](auto m, const char *in) {
                o(m, in);
                this->operator()(in, o);
              }});
  }
};
template <typename P> many(P)->many<P>;

template <typename P> struct oneOrMany {
  P p;
  template <typename O> void operator()(const char *in, O &&o) const {
    p(in, _o_{[&](int err) { o(err); },
              [&](auto m, const char *in) {
                o(m, in);
                many{p}(in, o);
              }});
  }
};
template <typename P> oneOrMany(P)->oneOrMany<P>;

} // namespace parse

namespace parse::xml {
inline auto Char = satisfy([](uint32_t c) {
  return c == 0x9 || c == 0xA || c == 0xD || (0x20 <= c && c <= 0xD7FF) ||
         (0xE000 <= c && c <= 0xFFFD) || (0x10000 <= c && c <= 0x10FFFF);
});
inline auto S = oneOrMany{satisfy([](uint32_t c) {
  // S	   ::=   	(#x20 | #x9 | #xD | #xA)+
  return c == 0x20 || c == 0x9 || c == 0xD || c == 0xA;
})};
} // namespace parse::xml

#include "zip.hpp"
#include <fstream>
#include <iostream>
#include <tuple>

auto main() -> int {
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

  parse::oneOrMany{parse::xml::Char}(
      "\00101!`აბAB", _o_{[](int) { std::cout << "error!\n"; },
                          [](std::string_view f, const char *rest) {
                            std::cout << '[' << f << "] [" << rest << "]\n";
                          }});
  parse::str("ა")("აბვ", _o_{[](int) { std::cout << "error\n"; },
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
