#ifndef ABO_UNION_HPP
#define ABO_UNION_HPP
#include "overloaded.hpp"
#include <functional>
#include <tuple>
#include <utility>
namespace abo {

template <typename... O> struct o;
template <typename O> struct o<O> : O {
  O _o;
  using O::operator();
  template <typename U> o(U &&u) : _o(std::forward<U>(u)) {}
};
template <typename O, typename... Rest>
struct o<O, Rest...> : o<O>, o<Rest...> {
  using o<O>::operator();
  using o<Rest...>::operator();
  template <typename U, typename... Us>
  o(U &&u, Us &&... us)
      : o<O>(std::forward<U>(u)), o<Rest...>(std::forward<Us>(us)...) {}
};
template <typename... O> o(O...)->o<O...>;

// template <typename O> struct o<void (*)(O &&)> {};
// template <typename O, typename... Rest> struct o<void (*)(O &&), Rest...> {};

template <typename... T> struct ray;
template <typename T> struct ray<T> { void operator()(T &&) const {}; };
template <typename T, typename... Rest>
struct ray<T, Rest...> : ray<T>, ray<Rest...> {
  using ray<T>::operator();
  using ray<Rest...>::operator();
};

template <typename T>
using is_pith_t =
    std::enable_if_t<std::is_invocable_r_v<void, T, void (*)(...)>>;

template <typename L, typename R, typename = is_pith_t<L>,
          typename = is_pith_t<R>>
constexpr decltype(auto) operator*(const L &l, const R &r) {
  return [l, r](auto &&o) {
    l([&o, &r](auto &&... a) {
      r([o = std::bind_front(o, std::forward<decltype(a)>(a)...)](
            auto &&... b) { std::invoke(o, std::forward<decltype(b)>(b)...); });
    });
  };
}
template <typename L, typename R, typename = is_pith_t<L>,
          typename = is_pith_t<R>>
constexpr decltype(auto) operator+(const L &l, const R &r) {
  return [l, r](auto &&o) {
    l([&o](auto &&... a) { std::invoke(o, std::forward<decltype(a)>(a)...); });
    r([&o](auto &&... b) { std::invoke(o, std::forward<decltype(b)>(b)...); });
  };
}

} // namespace abo
#endif