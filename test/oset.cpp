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

template <typename T, typename... R>
struct is_ray : std::conjunction<std::is_invocable_r<void, T, R>...>::type {};

template <typename T, typename... R>
struct is_pith : std::is_invocable_r<void, T, ray<R...>>::type {};

template <typename... C> struct if_ {
  using type = typename std::enable_if<std::conjunction_v<C...>>::type;
};

template <typename Pith> struct bark {
  Pith pith;

  template <typename U, typename = typename if_<is_pith<U, int>>::type>
  bark(U &&u) : pith(std::forward<U>(u)){};

  template <typename U, typename = typename if_<is_ray<U, int>>::type>
  void operator()(U &&) const {};
};
template <typename Pith> bark(Pith)->bark<Pith>;

static void t() {
  auto b = bark{[](auto &&o) { o(1); }};
  // b([](int) {});

  auto l = [](auto &&o) {
    o(1.f);
    o(1.1);
    o(1l);
    o("abc");
  };
  auto o = [](const char *) {};
  static_assert(is_pith<decltype(l), long, const char *, double>::value);
  static_assert(is_ray<decltype(o), const char *>::value);
}

template <typename F, typename... Rays> struct o_fmap;
template <typename F, typename L> struct o_fmap<F, L> {
  F f;
  template <typename Pith, typename = std::enable_if_t<std::is_invocable_r_v<
                               void, Pith, overloaded<ray<L>, F>>>>
  constexpr auto operator()(Pith &&_pith) const {
    return [
      pith = std::forward<Pith>(_pith), &f = this->f
    ]<typename O,
      typename = std::enable_if_t<std::is_invocable_r_v<void, O, L>>>(O && o) {
      auto o_ = overloaded{
          [&o](L i) { o(i); },
          [&f, &o]<typename A>(A &&a) {
            if constexpr (std::is_invocable_r_v<void,
                                                decltype(std::invoke(
                                                    f, std::forward<A>(a))),
                                                ray<>>) {
              std::invoke(f, std::forward<A>(a))(o);
            } else
              o(std::invoke(f, std::forward<A>(a)));
          }};
      pith(o_);
    };
  }
};
template <typename... L, typename F> constexpr auto fmap(F &&f) {
  return o_fmap<F, L...>{std::forward<F>(f)};
}

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
