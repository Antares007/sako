#include "pin.hpp"

template <typename Pith> struct purry {
  Pith pith;
  template <typename O> auto operator()(O &&o) const {
    if constexpr (std::is_invocable_r_v<void, Pith, O>)
      pith(static_cast<O &&>(o));
    else
      return purry<pin<Pith, O>>{pin{pith, static_cast<O &&>(o)}};
  }
};
template <typename Pith> purry(Pith)->purry<Pith>;
template <typename Pith, typename A> struct purry<pin<Pith, A>> {
  pin<Pith, A> pith;
  template <typename O> auto operator()(O &&o) const {
    if constexpr (std::is_invocable_r_v<void, Pith, O>)
      pith(static_cast<O &&>(o));
    else
      return purry<pin<pin<Pith, A>, O>>{pin{pith, static_cast<O &&>(o)}};
  }
};
template <typename Pith, typename A, typename B>
struct purry<pin<pin<Pith, A>, B>> {
  pin<pin<Pith, A>, B> pith;
  template <typename O> auto operator()(O &&o) const {
    if constexpr (std::is_invocable_r_v<void, Pith, O>)
      pith(static_cast<O &&>(o));
    else
      return purry<pin<pin<pin<Pith, A>, B>, O>>{
          pin{pith, static_cast<O &&>(o)}};
  }
};

#include "_o_.hpp"
#include <iostream>
int main() {
  int r = 0;

  purry{[](auto o, auto a, auto, int b, int c) { o(a, b + c); }}([](auto o) {
    o("A", "B");
  })(6)(9)(_o_{[](int) {}, [&](auto, auto a) { r = a; }});

  return r;
}
