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

#define OB(...) [__VA_ARGS__](const auto &o OB_NEXT_
#define OB_NEXT_(...) __VA_OPT__(,) __VA_ARGS__)

#define MOB(...)                                                               \
  template <typename O __VA_OPT__(,) __VA_ARGS__>                              \
  void operator()(const O &o MOB_NEXT_
#define MOB_NEXT_(...) __VA_OPT__(,) __VA_ARGS__) const noexcept
