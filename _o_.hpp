#pragma once

template <class... Ts> struct _o_ : Ts... { using Ts::operator()...; };
template <class... Ts> _o_(Ts...)->_o_<Ts...>;

template <typename... T> struct ray;
template <> struct ray<> {
  template <typename U> void operator()(U &&) const {}
};
template <typename T> struct ray<T> { void operator()(T &&) const {}; };
template <typename T, typename... Rest>
struct ray<T, Rest...> : ray<T>, ray<Rest...> {
  using ray<T>::operator();
  using ray<Rest...>::operator();
};
