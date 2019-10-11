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

template <typename L, typename F> struct fmap {
  F f;
  template <typename Pith, typename = std::enable_if_t<std::is_invocable_r_v<
                               void, Pith, abo::o<abo::ray<L>, proxy_ray<F>>>>>
  constexpr auto operator()(Pith &&_pith) const {
    return [pith = std::forward<Pith>(_pith), f = this->f](auto &&o) {
      pith(abo::o{
          [&o](L &&l) { o(std::forward<decltype(l)>(l)); },
          [&f, &o]<typename A>(A &&a) {
            using R = decltype(std::invoke(f, std::forward<A>(a)));
            if constexpr (std::is_invocable_r_v<void, R,
                                                abo::o<abo::ray<L>, any_ray>>)
              std::invoke(f, std::forward<A>(a))(o);
            else
              o(std::invoke(f, std::forward<A>(a)));
          }});
    };
  }
};


int main() {
  auto p =
      [](auto o) {
        if (false)
          o(A{0});
        else
          o(B{0});
      } //
      | fmap([](int) { return "hello"; });
}
