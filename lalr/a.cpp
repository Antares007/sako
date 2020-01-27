#include "g41.hpp"
#include <cxxabi.h>
#include <iostream>
#include <string>
#include <typeindex>

constexpr inline auto demangle = [](const char *name) {
  int stat = 0;
  auto buff = abi::__cxa_demangle(name, nullptr, nullptr, &stat);
  auto rez = std::string(stat ? name : buff);
  free(buff);
  return rez;
};

template <typename T> struct print;

template <typename V> struct index_ring {
  V v;
  template <typename O> void operator()(const O &o) const {
    v([&]<typename P>(P &&production) {
      o([&, production = static_cast<P &&>(production)](const auto &o) {
        size_t index = 0;
        production([&]<typename S>(S &&symbol) {
          if constexpr (parsec::is_parser_bark_v<S>)
            o(static_cast<S &&>(symbol), v, index++);
          else
            o(index_ring<S>{static_cast<S &&>(symbol)}, v, index++);
        });
      });
    });
  }
};
template <typename... Args> index_ring(Args...) -> index_ring<Args...>;

template <typename V, typename G> struct goto_ring {
  V v;
  G g;
  template <typename O> void operator()(const O &o) const {
    auto gtype_index = std::type_index(typeid(g));
    v([&]<typename P>(P &&production) {
      o([&, production = static_cast<P &&>(production)](const auto &o) {
        bool first = true;
        bool skip = false;
        production([&]<typename S>(S &&symbol) {
          if (skip)
            return;
          if (first) {
            first = false;
            skip = gtype_index != std::type_index(typeid(symbol));
          } else {
            if constexpr (parsec::is_parser_bark_v<S>)
              o(static_cast<S &&>(symbol));
            else
              o(goto_ring<S, G>{static_cast<S &&>(symbol), g});
          }
        });
      });
    });
  }
};
template <typename... Args> goto_ring(Args...) -> goto_ring<Args...>;

// innerhart
struct pith {
  template <typename S, typename V>
  void operator()(const S &&symbol, const V &variable, size_t index) const {
    (void(symbol));
    (void(variable));
    (void(index));
  }
  template <typename P> void operator()(const P &&production) const {
    production(*this);
  }
};
struct cpith {
  void operator()(error_ray *, int err) const {
    std::cerr << "Error: " << err << '\n';
  }
  template <typename N> void operator()(next_ray *, const N &n) const {
    std::cout << "Next!\n";
    n(*this);
  }
  template <typename V> void operator()(const V &v) const {
    std::cout << "V: " << v << '\n';
  }
};
int main() { //
  auto t = [](auto o) { o(0); } >>= [](auto o) { o(1); } >>=
      [](auto o) { o(2); };
  t(cpith{});
  std::cout << "done!\n\n";
  //  auto var = goto_ring{expr{}, expr{}};
  //
  //  //  var(pith{});
  //
  //  var([](auto p) {
  //    p([&](auto s) { std::cout << demangle(typeid(s).name()) << '\n'; });
  //  });

  return 9;
}
