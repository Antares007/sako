#include "../parsec.hpp"
template <typename T> struct print;
#include <iostream>

constexpr inline auto log = [](size_t ident, auto... args) {
  while (ident--)
    std::cout << ' ';
  ((std::cout << args), ...);
  std::cout << std::endl;
};

struct spith_state {
  const char *b = "";
  size_t pos = 0;
  bool error = false;
  bool done = false;
};
size_t ident = 0;

// this is pith!
struct spith {
  spith_state &s;
  template <typename S> void operator()(S symbol) const {
    if (s.error)
      return;
    if constexpr (parsec::is_parser_bark_v<decltype(symbol)>) {
      symbol(::rays{[&](error_ray *, int) { s.error = true; },
                    [&](size_t len) { s.pos += len; }},
             s.b + s.pos);
    } else {
      symbol(*this);
    }
  }
  template <typename P> void operator()(const char *n, P production) const {
    if (s.done || ident > 4)
      return;
    auto sd = spith_state{s.b + s.pos};
    auto i = ident++;
    log(i * 4, n, ">", s.b + s.pos);
    production(spith{sd});
    ident--;
    if (sd.error)
      return;
    s.pos = s.pos + sd.pos;
    // if (s.b[s.pos] == '\0') {
    //  s.done = true;
    //}
  }
};

#include "g41.hpp"
int main() {
  auto s = spith_state{"a+b*o"};
  S{}(spith{s});
  std::cout << "Pos:" << s.pos << ", E:" << s.error << '\n';
  // Є
  // auto log = ::rays{[](error_ray *, int err) { std::cerr << err <<
  // std::endl;
  // },
  //                  [](auto x) { std::cout << x << std::endl; }};
  // compile{}(log, E{}, input);
  return 8;
}
