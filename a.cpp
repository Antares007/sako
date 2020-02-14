#include "lalr/list_piths.hpp"
#include "purry.hpp"
#include <iostream>

template <typename... Ts> struct print;

int main() { //
  auto p =
      o::rec{[](const auto &rec, head_ray *, const auto &v, const auto &t) {
        if constexpr (std::is_invocable_r_v<void, decltype(v), void (*)(...)>)
          v(rec);
        else
          std::cout << v << '\n';
        if constexpr (!std::is_same_v<const decltype(nullptr) &, decltype(t)>)
          t(rec);
      }};
  auto np = nullptr;
  const auto &npr = np;
  L2(L1('a'), L2(3, 6.9))(concat_to_inners_pith{p, L2('a', "bo")});

  L2(1, 2)(concat_pith{p, npr});
  return 9;
}
