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
      o("T->FT'", [](auto o) {
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
  void operator()(O o, V v, const char *b) const {
    size_t ident = D * 2;
    bool done = false;
    (v)([&](auto name, auto production) {
      log(ident, name, ' ', done);
      if (done)
        return;
      else
        done = true;
      size_t length = 0;
      bool error = false;
      production([&](auto symbol) {
        if (error)
          return;
        auto rays = ::rays{[&](error_ray *, int) {
                             error = true;
                             done = false;
                           },
                           [&](size_t len) { length += len; }};
        if constexpr (std::is_invocable_r_v<void, decltype(symbol),
                                            decltype(parsec::rays),
                                            const char *>) {
          symbol(rays, b + length);
        } else {
          compile<D + 1>{}(rays, symbol, b + length);
        }
      });
      log(ident, error ? "Err" : "Ok");
      if (!error) {
        o(length);
      }
    });
    log(ident, "send error?", !done);
    if (!done)
      o(error_ray_v, -99);
  }
};

template <> struct compile<7> {
  template <typename O, typename V>
  void operator()(O o, V, const char *) const {
    log(7 * 2, "1024");
    o(error_ray_v, -1024);
  }
};

// this is pith!
struct spith {
  const char *b = "";
  size_t pos = 0;
  bool error = false;
  bool done = false;
  template <typename S> void operator()(S symbol) {
    if (error)
      return;
    if constexpr (parsec::is_parser_bark_v<decltype(symbol)>) {
      symbol(::rays{[&](error_ray *, int) { error = true; },
                    [&](size_t len) { pos += len; }},
             b + pos);
    } else {
      auto pith = spith{b + pos, 0, false};
      symbol(pith);
      error = pith.error;
      pos += pith.pos;
    }
  }
  template <typename P> void operator()(const char *n, P production) {
    if (done)
      return;
    std::cout << n << ':' << b + pos << "\n";
    auto pith = spith{b + pos, 0, false};
    production(pith);
    pos += pith.pos;
    done = (pith.b[pos] == '\0');
  }
};
#include <algorithm>
#include <set>
/*
E->TE'{
  T->FT'{
    F->(E){
    }
      F->id{
      }
        T'->*FT'{
          T'->Є{
          }
        }
  }
            E'->+TE'{
              E'->Є{
              }
            }
}
 */
#include <typeindex>
#include <typeinfo>
struct epith {
  std::set<std::type_index> &set;
  int &i;
  template <typename S> void operator()(S symbol) const {
    if constexpr (!parsec::is_parser_bark_v<decltype(symbol)>) {
      symbol(*this);
    }
  }
  template <typename P> void operator()(const char *n, P production) const {
    auto t = std::type_index(typeid(P));
    auto search = set.find(t);
    if (search != set.end())
      return;
    set.insert(t);
    int idn = i++;
    log(idn * 2, n, "{");
    production(*this);
    log(idn * 2, "}");
  }
};
int main() {
  // spith{"a+b*o"}
  auto set = std::set<std::type_index>();
  int i = 0;
  auto info = std::type_index(typeid(E));
  E{}(epith{set, i});
  // Є

  // const char *input = "a+b*o";
  // auto log = ::rays{[](error_ray *, int err) { std::cerr << err << std::endl;
  // },
  //                  [](auto x) { std::cout << x << std::endl; }};

  // compile{}(log, E{}, input);
  return 8;
}
