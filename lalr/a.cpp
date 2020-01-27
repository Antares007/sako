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

constexpr inline auto invocable = [](auto &&...) {};

template <typename L, typename R>
constexpr std::enable_if_t<
    std::disjunction_v<
        std::conjunction<std::is_invocable_r<void, L, decltype(invocable)>,
                         std::is_invocable_r<void, R, decltype(invocable)>>,
        std::conjunction<
            std::is_invocable_r<void, L, decltype(invocable), const char *>,
            std::is_invocable_r<void, R, decltype(invocable), const char *>>>,
    bool>
operator==(const L &l, const R &r) {
  return std::type_index(typeid(l)) == std::type_index(typeid(r));
}

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

int main() { //

  auto var = goto_ring{expr{}, expr{}};

  //  var(pith{});

  var([](auto p) {
    p([&](auto s) { std::cout << demangle(typeid(s).name()) << '\n'; });
  });

  auto aaa = expr{} == expr{};

  return aaa ? 9 : 3;
}
