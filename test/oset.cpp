#include "oset.hpp"

using namespace abo;

template <typename F> struct proxy_ray {
  F f;
  template <typename U>
  constexpr auto operator()(U &&u) const
      -> std::void_t<decltype(std::invoke(f, std::forward<U>(u)))> {
    std::invoke(f, std::forward<U>(u));
  };
};
template <typename F> proxy_ray(F)->proxy_ray<F>;

struct any_ray {
  template <typename U> constexpr void operator()(U &&) const {};
};

template <typename... F> struct o_fmap;
template <typename F> struct o_fmap<int, F> {
  F f;
  template <typename Pith,
            typename = std::enable_if_t<std::is_invocable_r_v<
                void, Pith, abo::o<abo::ray<int>, proxy_ray<F>>>>>
  constexpr auto operator()(Pith &&_pith) const {
    return [pith = std::forward<Pith>(_pith), f = this->f](auto &&o) {
      pith(abo::o{
          [&o](int i) { o(i); },
          [&f, &o]<typename A>(A &&a) {
            using R = decltype(std::invoke(f, std::forward<A>(a)));
            if constexpr (std::is_invocable_r_v<void, R,
                                                abo::o<abo::ray<int>, any_ray>>)
              std::invoke(f, std::forward<A>(a))(o);
            else
              o(std::invoke(f, std::forward<A>(a)));
          }});
    };
  }
};
template <typename F> constexpr o_fmap<int, F> fmap(F &&f) {
  return o_fmap<int, F>{std::forward<F>(f)};
}

NewType(A, int);
NewType(B, int);
int main() {
  auto p = fmap([](B) { return 1.2; })([](auto o) {
    if (false)
      o(1);
    else
      o(B{0});
  });
}
