#include "pin.hpp"

template <typename Pith> struct purry {
  Pith pith;
  template <typename T, typename U> auto operator()(T &&t, U &&u) const {
    return this->operator()(static_cast<T &&>(t))(static_cast<U &&>(u));
  }
  template <typename T, typename U, typename W>
  auto operator()(T &&t, U &&u, W &&w) const {
    return this->operator()(static_cast<T &&>(t))(static_cast<U &&>(u))(
        static_cast<W &&>(w));
  }
  template <typename O> auto operator()(O &&o) const {
    if constexpr (std::is_invocable_r_v<void, Pith, O>)
      pith(static_cast<O &&>(o));
    else
      return purry<pin<Pith, O>>{pin<Pith, O>{pith, static_cast<O &&>(o)}};
  }
};
template <typename Pith, typename A> struct purry<pin<Pith, A>> {
  pin<Pith, A> pith;
  template <typename T, typename U> auto operator()(T &&t, U &&u) const {
    return this->operator()(static_cast<T &&>(t))(static_cast<U &&>(u));
  }
  template <typename O> auto operator()(O &&o) const {
    if constexpr (std::is_invocable_r_v<void, Pith, O>)
      pith(static_cast<O &&>(o));
    else
      return purry<pin<pin<Pith, A>, O>>{
          pin<pin<Pith, A>, O>{pith, static_cast<O &&>(o)}};
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
          pin<pin<pin<Pith, A>, B>, O>{pith, static_cast<O &&>(o)}};
  }
};
template <typename Pith> purry(Pith)->purry<Pith>;
