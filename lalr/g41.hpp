#pragma once
#include "terminals.hpp"
#define Derives MBark()()
#define Production [](const auto &o)

struct next_ray;
template <typename...> struct cont;
template <typename... Args> cont(Args...) -> cont<Args...>;

template <typename L, typename R> struct cont<L, R> {
  L l;
  R r;
  template <typename O, typename... Args>
  void operator()(const O &o, Args &&... args) const { //
    bool error = false;
    l(::rays{[&](error_ray *tag, int err) {
               error = true;
               o(tag, err);
             },
             [&]<typename V>(V &&v) { o(static_cast<V &&>(v)); }},
      static_cast<Args &&>(args)...);
    if (!error)
      o(static_cast<next_ray *>(nullptr), r);
  }
};

template <typename L, typename R> constexpr auto operator>>=(L &&l, R &&r) {
  return cont<L, R>{static_cast<L &&>(l), static_cast<R &&>(r)};
}

struct expr {
  template <typename O> void operator()(const O &o) const {
    o(expr{} >>= PLUS{} >>= term{});
    o(term{});
  }
  struct term {
    template <typename O> void operator()(const O &o) const {
      o(term{} >>= MUL{} >>= factor{});
      o(factor{});
    }
  };
  struct factor {
    template <typename O> void operator()(const O &o) const {
      o(LPAREN{} >>= expr{} >>= RPAREN{});
      o(ID{});
    }
  };
};
