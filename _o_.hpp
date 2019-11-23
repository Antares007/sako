#pragma once
#include <type_traits>
template <class... Ts> struct _o_ : Ts... {
  using Ts::operator()...;
  template <typename Pith,
            typename = std::enable_if_t<std::is_invocable_r_v<void, Pith, _o_>>>
  void operator()(Pith &&pith) const {
    pith(*this);
  }
};
template <class... Ts> _o_(Ts...)->_o_<Ts...>;

template <typename... T> struct ray {
  void operator()(T...) const {}
};
template <> struct ray<> {
  template <typename U> void operator()(U &&) const {}
};

template <typename... T> struct print;
