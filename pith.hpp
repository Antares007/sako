#pragma once

template <class... Ts> struct rays : Ts... { using Ts::operator()...; };
template <class... Ts> rays(Ts...)->rays<Ts...>;

#define P(...) [__VA_ARGS__](const auto &o P_NEXT_
#define P_NEXT_(...) __VA_OPT__(,) __VA_ARGS__)

#define MP(...)                                                                \
  template <typename O __VA_OPT__(,) __VA_ARGS__>                              \
  void operator()(const O &o MP_NEXT_
#define MP_NEXT_(...) __VA_OPT__(,) __VA_ARGS__) const noexcept
