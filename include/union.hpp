#ifndef ABO_UNION_HPP
#define ABO_UNION_HPP
#include "overloaded.hpp"
#include <utility>
namespace abo {

template <typename P> struct union_fn {
  P pith;
  template <typename O>
  constexpr auto operator()(O &&o) const -> decltype(pith(std::declval<O>())) {
    bool firstrun = true;
    return pith([&o, &firstrun](auto &&... x) {
      if (firstrun) {
        firstrun = false;
        o(std::forward<decltype(x)>(x)...);
      }
    });
  }
};
template <typename P> union_fn(P)->union_fn<P>;

} // namespace abo
#endif
