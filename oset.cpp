#include "overloaded.hpp"
#include <utility>
template <typename... Ts> struct print;
template <typename... Fns> struct oset;

template <typename T> struct oset<T> : T {
  using T::operator();
  template <typename U> constexpr oset(U &&u) : T(std::forward<U>(u)) {}
};

template <typename T> struct oset<void (*)(T &&)> {
  using F = void (*)(T &&);
  F f;
  template <typename U> oset(U &&u) : f(std::forward<U>(u)) {}
  template <typename U> constexpr void operator()(U &&u) const {
    f(std::forward<U>(u));
  }
};

template <typename T, typename... Rest>
struct oset<T, Rest...> : T, oset<Rest...> {

  using T::operator();
  using oset<Rest...>::operator();

  template <typename U, typename... Us>
  constexpr oset(U &&u, Us &&... us)
      : T(std::forward<U>(u)), oset<Rest...>(std::forward<Us>(us)...) {}
};

template <typename T, typename... Rest>
struct oset<void (*)(T &&), Rest...> : oset<Rest...> {
  using F = void (*)(T &&);
  F f;
  using oset<Rest...>::operator();

  template <typename U, typename... Us>
  constexpr oset(U &&u, Us &&... us)
      : oset<Rest...>(std::forward<Us>(us)...), f(std::forward<U>(u)) {}
  template <typename U> constexpr void operator()(U &&u) const {
    f(std::forward<U>(u));
  }
};

template <typename... Ts> oset(Ts...)->oset<Ts...>;

template <typename... types> struct union_bark;

template <typename T> struct union_bark<T> {
  using O = oset<void (*)(T &&)>;
  using Pith = void (*)(const O &);
  Pith pith;

  //  template <typename...Fs>
  //  constexpr auto operator()( Fs &&...fs)
  //      -> decltype(f(std::declval<T>())) const { //
  //    T l = {};
  //    pith([&l](T t) { l = std::move(t); });
  //    return f(std::move(l));
  //  }
};

using bark = union_bark<int>;
int main() {
  auto os = oset{+[](int &&) {}, +[](float &&) {}};

  auto r = union_bark<int, float>{+[](const union_bark<int, float>::O &o) {
    o(1);
    o(1);
    //  print<decltype(a(10.f))>p;
  }};
}
