#include "_o_.hpp"
#include "lift.hpp"
#include <functional>

template <typename Pith, typename A> struct pin {
  Pith pith;
  A a;
  template <typename O, typename... Rest>
  constexpr void operator()(const O &o, Rest &&... rest) const {
    auto r = _o_{[&](int err) { o(err); },
                 [&, ... rest = std::forward<Rest>(rest)](auto &&... a) {
                   pith(o, std::forward<decltype(a)>(a)..., rest...);
                 }};
    if constexpr (std::is_invocable_r_v<void, A, decltype(r)>)
      a(r);
    else
      pith(o, a, rest...);
  }
};
template <typename Pith, typename A> pin(Pith, A)->pin<Pith, A>;

template <typename> struct is_pin : std::false_type {};
template <typename T, typename A> struct is_pin<pin<T, A>> : std::true_type {};

template <typename L, typename R,
          typename = std::enable_if_t<std::disjunction_v<
              lift::is_lift<std::decay_t<L>>, is_pin<std::decay_t<L>>>>
          /**/>
constexpr auto operator^(L &&l, R &&r) {
  return pin{std::forward<L>(l), std::forward<R>(r)};
}
