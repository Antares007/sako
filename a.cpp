#include <iostream>
template <typename O> struct var {
  O o;
  void operator()(auto &&... args) const {
    o(var{o}, static_cast<decltype(args) &&>(args)...);
  }
};
template <typename O> var(O) -> var<O>;

int main() { //
  return 9;
}
