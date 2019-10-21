#include "_o_.hpp"
#include <functional>


auto pstr(const char *str) {
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
auto utf8codepoint(const char *s, uint32_t *out_codepoint) -> const char * {
  if (0xf0 == (0xf8 & s[0])) {
    // 4 byte utf8 codepoint
    *out_codepoint = ((0x07 & s[0]) << 18) | ((0x3f & s[1]) << 12) |
                     ((0x3f & s[2]) << 6) | (0x3f & s[3]);
    s += 4;
  } else if (0xe0 == (0xf0 & s[0])) {
    // 3 byte utf8 codepoint
    *out_codepoint =
        ((0x0f & s[0]) << 12) | ((0x3f & s[1]) << 6) | (0x3f & s[2]);
    s += 3;
  } else if (0xc0 == (0xe0 & s[0])) {
    // 2 byte utf8 codepoint
    *out_codepoint = ((0x1f & s[0]) << 6) | (0x3f & s[1]);
    s += 2;
  } else {
    // 1 byte utf8 codepoint otherwise
    *out_codepoint = s[0];
    s += 1;
  }
  return s;
}
template <typename F> auto satisfy(F &&f) {
  return [f = std::forward<F>(f)]<typename O>(const char *, O &&) { ///

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

#include "zip.hpp"
#include <fstream>
#include <iostream>
#include <tuple>

template <typename... Ts> struct print;
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
                         [&](auto buff, auto size) {
                           std::cout << name << " - " << size << "\n"
                                     << buff << "\n";
                         }});
                 }});

  pstr("ა")("აბვ", _o_{[](int) { std::cout << "error\n"; },
                       [](const char *f, const char *rest) {
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
