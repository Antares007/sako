#include "../parsec.hpp"
template <typename T> struct print;
#define Derives template <typename O> void operator()(O o) const
constexpr inline auto PLUS = parsec::str{"+"};
constexpr inline auto MUL = parsec::str{"*"};
constexpr inline auto LPAREN = parsec::str{"("};
constexpr inline auto RPAREN = parsec::str{")"};
constexpr inline auto ID =
    parsec::chr{[](auto c) { return c <= 'a' && c <= 'z'; }};
constexpr inline auto Є = parsec::str{""};

struct E {
  Derives {
    o("E->TE'", [](auto o) {
      o(T{});
      o(E_{});
    });
  }
  struct E_ {
    Derives {
      o("E'->+TE'", [](auto o) {
        o(PLUS);
        o(T{});
        o(E_{});
      });
      o("E'->Є", [](auto o) { o(Є); });
    }
  };
  struct T {
    Derives {
      o("T->FU", [](auto o) {
        o(F{});
        o(T_{});
      });
    }
    struct T_ {
      Derives {
        o("T'->*FT'", [](auto o) {
          o(MUL);
          o(F{});
          o(T_{});
        });
        o("T'->Є", [](auto o) { o(Є); });
      }
    };
    struct F {
      Derives {
        o("F->(E)", [](auto o) {
          o(LPAREN);
          o(E{});
          o(RPAREN);
        });
        o("F->id", [](auto o) { o(ID); });
      }
    };
  };
};
#include <iostream>
constexpr inline auto log = [](size_t ident, auto... args) {
  while (ident--)
    std::cout << ' ';
  ((std::cout << args), ...);
  std::cout << std::endl;
};
template <size_t D = 0> struct compile {
  template <typename O, typename V>
  void operator()(O o, V v, const char *b, size_t ident) const {
    bool done = false;
    (v)([&](auto name, auto production) {
      log(ident, name, " done: ", done);
      if (done)
        return;
      size_t length = 0;
      bool error = false;
      production([&](auto symbol) {
        log(ident, error ? 'x' : '.');
        if (error)
          return;
        auto rays = ::rays{[&](error_ray *, int) { error = true; },
                           [&](size_t len) { length += len; }};
        if constexpr (std::is_invocable_r_v<void, decltype(symbol),
                                            decltype(parsec::rays),
                                            const char *>) {
          symbol(rays, b + length);
        } else {
          compile<D + 1>{}(rays, symbol, b + length, ident + 4);
        }
      });
      if (!error) {
        done = true;
        o(length);
      }
    });
    if (!done)
      o(error_ray_v, -99);
  }
};

template <> struct compile<7> {
  template <typename O, typename V>
  void operator()(O o, V, const char *, size_t ident) const {
    log(ident, "1024");
    o(error_ray_v, -1024);
  }
};

int main() {
  // Є
  const char *input = "a+b*o";
  auto log = ::rays{[](error_ray *, int err) { std::cerr << err << std::endl; },
                    [](auto x) { std::cout << x << std::endl; }};

  compile{}(log, E{}, input, 0);
  return 8;
}
