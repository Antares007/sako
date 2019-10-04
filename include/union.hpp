#ifndef ABO_UNION_HPP
#define ABO_UNION_HPP
#include "overloaded.hpp"
#include <functional>
#include <tuple>
#include <utility>
namespace abo {

constexpr inline auto o_1 = [](auto &&) {};
constexpr inline auto o_n = [](auto &&...) {};
using o_1_t = decltype(o_1);
using o_n_t = decltype(o_n);

template <typename P> struct union_fn {
  P pith;

  template <typename U,
            typename = std::enable_if_t<std::is_invocable_v<U, o_n_t>>>
  union_fn(U &&u) : pith(std::forward<U>(u)) {}

  template <typename O>
  constexpr auto operator()(O &&o) const -> decltype(pith(o)) {
    bool firstrun = true;
    return pith([&o, &firstrun](auto &&... a) {
      if (firstrun) {
        firstrun = false;
        std::invoke(o, std::forward<decltype(a)>(a)...);
      }
    });
  }
};

template <typename L, typename R,
          typename = std::enable_if_t<std::is_invocable_r_v<void, L, o_n_t>>,
          typename = std::enable_if_t<std::is_invocable_r_v<void, R, o_n_t>>>
constexpr decltype(auto) operator+(const L &l, const R &r) {
  return union_fn{[l = l.pith, r = r.pith](auto &&o) {
    l([&o, &r](auto &&... a) {
      r([o = std::bind_front(o, std::forward<decltype(a)>(a)...)](
            auto &&... b) { std::invoke(o, std::forward<decltype(b)>(b)...); });
    });
  }};
}
template <typename P> union_fn(P)->union_fn<P>;

} // namespace abo
#endif
