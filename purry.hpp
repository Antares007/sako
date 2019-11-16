#include "pin.hpp"

template <typename> struct purry;
template <typename Pith> purry(Pith)->purry<Pith>;

template <typename Pith> struct purry {
  Pith pith;
  template <typename O> auto operator()(O &&o) const {
    if constexpr (std::is_invocable_r_v<void, Pith, O>)
      pith(static_cast<O &&>(o));
    else
      return purry<pin<Pith, O>>{pin<Pith, O>{pith, static_cast<O &&>(o)}};
  }
};
template <typename Pith, typename A> struct purry<pin<Pith, A>> {
  pin<Pith, A> pith;
  template <typename O> auto operator()(O &&o) const {
    if constexpr (std::is_invocable_r_v<void, Pith, O, A>) {
      pith(static_cast<O &&>(o));
    } else {
      return purry<pin<pin<Pith, A>, O>>{
          pin<pin<Pith, A>, O>{pith, static_cast<O &&>(o)}};
    }
  }
};
template <typename Pith, typename A, typename B>
struct purry<pin<pin<Pith, A>, B>> {
  pin<pin<Pith, A>, B> pith;
  template <typename O> auto operator()(O &&o) const {
    if constexpr (std::is_invocable_r_v<void, Pith, O, A, B>)
      pith(static_cast<O &&>(o));
    else {
      return purry<pin<pin<pin<Pith, A>, B>, O>>{
          pin<pin<pin<Pith, A>, B>, O>{pith, static_cast<O &&>(o)}};
    }
  }
};
