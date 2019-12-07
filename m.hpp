#pragma once

#define MB(name) struct name##_fn

#define ME(name)                                                               \
  template <typename Pith> name##_fn(Pith)->name##_fn<Pith>;                   \
  constexpr inline auto name = [](auto &&pith) {                               \
    return purry{name##_fn{static_cast<decltype(pith) &&>(pith)}};             \
  }

#define MO(...)                                                                \
  template <typename O>                                                        \
  constexpr void operator()(O o, __VA_ARGS__) const noexcept {                 \
    M_ARGS_
#define M_ARGS_(...)                                                           \
  o(__VA_ARGS__);                                                              \
  }

#define G(name) template <typename T> name(T)->name<T>;

#define UFB(name)                                                              \
  template <typename A> struct name##_fn {                                     \
    A a
#define UFE(name, ...)                                                         \
  }                                                                            \
  ;                                                                            \
  constexpr inline auto name =                                                 \
      []<typename A,                                                           \
         typename = std::enable_if_t<std::conjunction_v<__VA_ARGS__>>>(A &&    \
                                                                       a) {    \
    return name##_fn<A>{static_cast<A &&>(a)};                                 \
  }
#define BFB(name)                                                              \
  template <typename A, typename B> struct name##_fn {                         \
    A a;                                                                       \
    B b
#define BFE(name, ...)                                                         \
  }                                                                            \
  ;                                                                            \
  constexpr inline auto name =                                                 \
      []<typename A, typename B,                                               \
         typename = std::enable_if_t<std::conjunction_v<__VA_ARGS__>>>(        \
          A && a, B && b) {                                                    \
    return name##_fn<A, B>{static_cast<A &&>(a), static_cast<B &&>(b)};        \
  }
#define BOP(oper, ...)                                                         \
  template <typename L, typename R,                                            \
            typename = std::enable_if_t<std::conjunction_v<__VA_ARGS__>>>      \
  constexpr auto operator oper(L &&l, R &&r)
