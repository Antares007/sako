#pragma once

template <class... Ts> struct _o_ : Ts... { using Ts::operator()...; };
template <class... Ts> _o_(Ts...)->_o_<Ts...>;

template <typename... T> struct ray {
  void operator()(T...) const {}
};
template <> struct ray<> {
  template <typename U> void operator()(U &&) const {}
};
