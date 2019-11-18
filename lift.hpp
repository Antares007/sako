#pragma once

#include "purry.hpp"

template <typename... Args> struct lift;
template <typename T, typename... Args> struct lift<T *, Args...> {
  int (*ctor)(T **, Args...);
  void (*dtor)(T *);
  template <typename O> void operator()(const O &o, Args... args) const {
    T *pv = nullptr;
    if (int e = ctor(&pv, args...))
      o(e);
    else {
      o(pv);
      dtor(pv);
    }
  }
};
template <typename T, typename... Args> struct lift<T, Args...> {
  int (*ctor)(T *, Args...);
  template <typename O> void operator()(const O &o, Args... args) const {
    T v{};
    if (int e = ctor(&v, args...))
      o(e);
    else
      o(&v);
  }
};
template <typename T, typename... Args>
lift(int (*)(T **, Args...), void (*)(T *))->lift<T *, Args...>;
template <typename T, typename... Args>
lift(int (*)(T *, Args...))->lift<T, Args...>;

template <typename T, typename R, typename... Args>
constexpr auto operator^(lift<T, Args...> l, R &&r) {
  return purry{l, static_cast<R &&>(r)};
}
