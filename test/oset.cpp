#include <functional>
template <typename... Names> struct print;
template <class... O> struct overloaded : O... { using O::operator()...; };
template <class... O> overloaded(O...)->overloaded<O...>;

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

template <typename F, typename... Rays> struct o_fmap;
template <typename... L, typename F> constexpr o_fmap<F, L...> fmap(F &&f) {
  return o_fmap<F, L...>{std::forward<F>(f)};
}
template <typename F, typename L> struct o_fmap<F, L> {
  F f;
  template <typename Pith, typename = std::enable_if_t<std::is_invocable_r_v<
                               void, Pith, overloaded<ray<L>, F>>>>
  constexpr auto operator()(Pith &&_pith) const {
    return [
      pith = std::forward<Pith>(_pith), this, &f = this->f
    ]<typename O,
      typename = std::enable_if_t<std::is_invocable_r_v<void, O, L>>>(O && o) {
      pith(overloaded{
          [&o](L i) { o(i); },
          [&f, &o, this]<typename A>(A &&a) {
            using R = decltype(std::invoke(f, std::forward<A>(a)));
            if constexpr (std::is_invocable_r_v<void, R,
                                                overloaded<ray<L>, F>>) {
              print<decltype(this)> p;
              
            } else
              o(std::invoke(f, std::forward<A>(a)));
          }});
    };
  }
};

#include <cstdio>
struct A {};
struct B {};
struct O {};
template <typename A, typename F>
constexpr auto operator|(A &&a, F &&f)
    -> decltype(std::invoke(std::forward<F>(f), std::forward<A>(a))) {
  return std::invoke(std::forward<F>(f), std::forward<A>(a));
}
int main() {
  auto p =
      [](auto o) {
        int i = -1;
        if (i < 0)
          o(1);
        else if (i == 0)
          o(B{});
        else if (i > 0)
          o(O{});
      } |
      fmap<int>(overloaded{[](B &&b) { return std::move(b); },
                           [](O &&) { return A{}; }});
  p(overloaded{
      [](A &&) { puts("a"); },
      [](int) { puts("o"); },
      [](B &&) { puts("b"); },
  });
  auto p2 = [](auto o) {
    if (false)
      o(A{});
    else
      o(O{});
  } | fmap<A>([](O &&) {
              return [](auto o) {
                if (false)
                  o(A{});
                else
                  o(O{});
              };
            });
  p2(overloaded{[](A) { puts("a"); }, [](O) { puts("o"); }});
}
