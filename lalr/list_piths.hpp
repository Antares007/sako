#pragma once
#include "mb.hpp"
#include <type_traits>

template <typename O, typename B> struct concat_pith {
  const O &o;
  const B &b;
  void operator()(head_ray *tag, Car h, Car t) const {
    if constexpr (std::is_same_v<decltype(nullptr), B>)
      o(tag, h, t);
    else if constexpr (std::is_same_v<const decltype(nullptr) &, decltype(t)>)
      o(tag, h, b);
    else
      o(tag, h, [&](Car o) { t(concat_pith<decltype(o), B>{o, b}); });
  }
};
template <typename O, typename B> concat_pith(O, B) -> concat_pith<O, B>;

template <typename O, typename B> struct concat_to_inners_pith {
  const O &o;
  const B &b;
  void operator()(head_ray *tag, Car h, Car t) const {
    if constexpr (std::is_same_v<decltype(nullptr), B>)
      o(tag, h, t);
    else {
      auto l = [&](Car o) { h(concat_pith{o, b}); };
      if constexpr (std::is_same_v<const decltype(nullptr) &, decltype(t)>)
        o(tag, l, t);
      else
        o(tag, l, [&](Car o) {
          t(concat_to_inners_pith<decltype(o), B>{o, b});
        });
    }
  }
};
template <typename O, typename B>
concat_to_inners_pith(O, B) -> concat_to_inners_pith<O, B>;
